#include "Sound.h"
#include "../Exception.h"

#include <Xli.h>
#include <XliPlatform.h>

#include <AudioToolbox/AudioToolbox.h>

#import <AVFoundation/AVFoundation.h>

namespace Uno
{
    namespace Audio
    {
        class AudioPacketBuffer
        {
            Xli::Array<UInt32> buffersSizeBytes;
            Xli::Array<Byte*> buffers;
            Xli::Array<UInt32> bufferPacketNums;
            Xli::Array<AudioStreamPacketDescription*> bufferPacketDescs;
            
        public:
            AudioStreamBasicDescription DataFormat;
            UInt32 BufferSizeBytes;
            UInt32 NumPacketsToRead;
            UInt32 CookieSize;
            char* Cookie;
            UInt32 AudioChannelLayoutSize;
            AudioChannelLayout* AudioChannelLayout;
            
            AudioPacketBuffer(const char* filename)
            {
                NSString* string = [NSString stringWithFormat:@"%@/data/%@", [[NSBundle mainBundle] resourcePath], [NSString stringWithUTF8String:filename]];
                
                NSURL* url = [NSURL fileURLWithPath:string];
                //[string release];
                
                AudioFileID file;
                if (AudioFileOpenURL((CFURLRef)url, kAudioFileReadPermission, 0, &file) != noErr)
                {
                    Xli::ErrorPrintLine("ERROR: Failed to open audio file.");
                    //[url release];
                    return;
                }
                //[url release];
                
                UInt32 size = sizeof(DataFormat);
                if (AudioFileGetProperty(file, kAudioFilePropertyDataFormat, &size, &DataFormat) != noErr)
                {
                    Xli::ErrorPrintLine("ERROR: Failed to get audio file properties.");
                    return;
                }
                
                UInt32 maxPacketSize;
                size = sizeof(maxPacketSize);
                if (AudioFileGetProperty(file, kAudioFilePropertyPacketSizeUpperBound, &size, &maxPacketSize) != noErr)
                {
                    Xli::ErrorPrintLine("ERROR: Failed to read max packet size.");
                    return;
                }
                
                if (DataFormat.mFramesPerPacket)
                {
                    Float64 numPacketsForTime = DataFormat.mSampleRate / DataFormat.mFramesPerPacket * .5; // Half-second buffers
                    BufferSizeBytes = numPacketsForTime * maxPacketSize;
                }
                else
                {
                    BufferSizeBytes = maxPacketSize;
                }
                NumPacketsToRead = BufferSizeBytes / maxPacketSize;
                
                CookieSize = sizeof(UInt32);
                OSStatus result = AudioFileGetPropertyInfo(file, kAudioFilePropertyMagicCookieData, &CookieSize, NULL);
                if (!result && CookieSize)
                {
                    Cookie = new char[CookieSize];
                    if (AudioFileGetProperty(file, kAudioFilePropertyMagicCookieData, &CookieSize, Cookie) != noErr)
                    {
                        Xli::ErrorPrintLine("ERROR: Failed to read cookie data.");
                        return;
                    }
                }
                else
                {
                    CookieSize = 0;
                    Cookie = 0;
                }
                
                result = AudioFileGetProperty(file, kAudioFilePropertyChannelLayout, &AudioChannelLayoutSize, NULL);
                if (result == noErr && size > 0)
                {
                    AudioChannelLayout = new ::AudioChannelLayout();
                    if (AudioFileGetProperty(file, kAudioFilePropertyChannelLayout, &AudioChannelLayoutSize, AudioChannelLayout) != noErr)
                    {
                        Xli::ErrorPrintLine("ERROR: Failed to read channel layout.");
                        return;
                    }
                }
                else
                {
                    AudioChannelLayoutSize = 0;
                    AudioChannelLayout = 0;
                }
                
                Byte* tempBuffer = new Byte[BufferSizeBytes];
                AudioStreamPacketDescription* tempPacketDescs = new AudioStreamPacketDescription[NumPacketsToRead];
                SInt64 currentPacket = 0;
                while (true)
                {
                    UInt32 numBytes;
                    UInt32 numPackets = NumPacketsToRead;
                    if (AudioFileReadPackets(file, false, &numBytes, tempPacketDescs, currentPacket, &numPackets, tempBuffer) != noErr)
                    {
                        Xli::ErrorPrintLine("ERROR: Failed to read audio packets.");
                        delete tempBuffer;
                        delete tempPacketDescs;
                        return;
                    }
                    if (!numPackets) break;
                    buffersSizeBytes.Add(numBytes);
                    Byte* bufferData = new Byte[numBytes];
                    for (int i = 0; i < numBytes; i++) bufferData[i] = tempBuffer[i];
                    buffers.Add(bufferData);
                    bufferPacketNums.Add(numPackets);
                    AudioStreamPacketDescription* descData = new AudioStreamPacketDescription[numPackets];
                    for (int i = 0; i < numPackets; i++) descData[i] = tempPacketDescs[i];
                    bufferPacketDescs.Add(descData);
                    currentPacket += numPackets;
                }
                delete tempBuffer;
                delete tempPacketDescs;
                
                AudioFileClose(file);
            }
            
            ~AudioPacketBuffer()
            {
                for (int i = 0; i < buffers.Length(); i++) delete buffers[i];
                for (int i = 0; i < bufferPacketDescs.Length(); i++) delete bufferPacketDescs[i];
                if (Cookie) delete Cookie;
                if (AudioChannelLayout) delete AudioChannelLayout;
            }
            
            void ReadBuffer(UInt32 bufferIndex, UInt32* numBytes, AudioStreamPacketDescription* descs, UInt32* numPackets, void* output)
            {
                if (bufferIndex >= buffers.Length())
                {
                    *numBytes = *numPackets = 0;
                    return;
                }
                
                *numBytes = buffersSizeBytes[bufferIndex];
                *numPackets = bufferPacketNums[bufferIndex];
                for (int i = 0; i < *numPackets; i++) descs[i] = bufferPacketDescs[bufferIndex][i];
                for (int i = 0; i < *numBytes; i++) ((Byte*)output)[i] = buffers[bufferIndex][i];
            }
        };
        
        class AudioQueueChannel : public Channel
        {
            const static int numBuffers = 3;
            
            AudioPacketBuffer* packetBuffer;
            AudioQueueRef queue;
            AudioQueueBufferRef buffers[numBuffers];
            bool playing;
            bool finished;
            bool looping;
            UInt32 currentBuffer;
            
            static void bufferCallback(void* userData, AudioQueueRef aq, AudioQueueBufferRef aqb)
            {
                
                
                AudioQueueChannel* c = (AudioQueueChannel*)userData;
                
                if (!c->playing || c->finished) return;
                
                c->packetBuffer->ReadBuffer(c->currentBuffer, &(aqb->mAudioDataByteSize), aqb->mPacketDescriptions, &(aqb->mPacketDescriptionCount), aqb->mAudioData);
                if (aqb->mPacketDescriptionCount > 0)
                {
                    if (AudioQueueEnqueueBuffer(aq, aqb, 0, NULL) != noErr) Xli::ErrorPrintLine("ERROR: Failed to enqueue audio buffer.");
                    c->currentBuffer++;
                }
                else
                {
                    if (c->looping)
                    {
                        c->currentBuffer = 0;
                        bufferCallback(userData, aq, aqb);
                    }
                    else
                    {
                        c->finished = true;
                    }
                }
            }
            
        public:
            AudioQueueChannel(AudioPacketBuffer* packetBuffer, bool loop)
            {
                this->packetBuffer = packetBuffer;
                
                if (AudioQueueNewOutput(&packetBuffer->DataFormat, bufferCallback, this, CFRunLoopGetCurrent(), kCFRunLoopCommonModes, 0, &queue) != noErr)
                {
                    Xli::ErrorPrintLine("ERROR: Failed to create output queue.");
                    return;
                }
                
                UInt32 barf = kAudioQueueHardwareCodecPolicy_PreferSoftware;
                AudioQueueSetProperty(queue, kAudioQueueProperty_HardwareCodecPolicy, &barf, sizeof(barf));
                
                if (packetBuffer->CookieSize)
                {
                    if (AudioQueueSetProperty(queue, kAudioQueueProperty_MagicCookie, packetBuffer->Cookie, packetBuffer->CookieSize) != noErr)
                    {
                        Xli::ErrorPrintLine("ERROR: Failed to set cookie data on queue.");
                        return;
                    }
                }
                
                if (packetBuffer->AudioChannelLayoutSize)
                {
                    if (AudioQueueSetProperty(queue, kAudioQueueProperty_MagicCookie, &packetBuffer->AudioChannelLayout, packetBuffer->AudioChannelLayoutSize) != noErr)
                    {
                        Xli::ErrorPrintLine("ERROR: Failed to set channel layout on queue.");
                        return;
                    }
                }
                
                bool isVbr = (packetBuffer->DataFormat.mBytesPerPacket == 0 || packetBuffer->DataFormat.mFramesPerPacket == 0);
                for (int i = 0; i < numBuffers; i++)
                {
                    if (AudioQueueAllocateBufferWithPacketDescriptions(queue, packetBuffer->BufferSizeBytes, isVbr ? packetBuffer->NumPacketsToRead : 0, &buffers[i]) != noErr)
                    {
                        Xli::ErrorPrintLine("ERROR: Failed to allocate buffer.");
                        return;
                    }
                }
                
                AudioQueueSetParameter(queue, kAudioQueueParam_Volume, 1.0);
                
                this->looping = loop;
                playing = true;
                finished = false;
                currentBuffer = 0;
                for (int i = 0; i < numBuffers; i++) bufferCallback(this, queue, buffers[i]);
                OSStatus err = AudioQueueStart(queue, NULL);
                if (err != noErr)
                {
                    Xli::ErrorPrintLine("ERROR: Failed to start playback.");
                    return;
                }
            }
            
            virtual ~AudioQueueChannel()
            {
                if (playing) AudioQueueStop(queue, true);
                AudioQueueDispose(queue, true);
            }
            
            virtual double GetPosition()
            {
                AudioTimeStamp timeStamp;
                AudioQueueGetCurrentTime(queue, NULL, &timeStamp, NULL);
                return (double)timeStamp.mSampleTime / packetBuffer->DataFormat.mSampleRate;
            }
            virtual void SetPosition(double value)
            {
                // TODO
            }
            
            virtual float GetPan()
            {
                float res;
                AudioQueueGetParameter(queue, kAudioQueueParam_Pan, &res);
                return res;
            }
            virtual void SetPan(float value)
            {
                AudioQueueSetParameter(queue, kAudioQueueParam_Pan, value);
            }
            
            virtual float GetVolume()
            {
                float res;
                AudioQueueGetParameter(queue, kAudioQueueParam_Volume, &res);
                return res;
            }
            virtual void SetVolume(float value)
            {
                AudioQueueSetParameter(queue, kAudioQueueParam_Volume, value);
            }
            
            virtual bool GetPause()
            {
                return false; // TODO
            }
            virtual void SetPause(bool value)
            {
                // TODO
            }
            
            virtual bool GetIsPlaying()
            {
                return playing;
            }
            virtual bool GetIsFinished()
            {
                return finished;
            }
            
            virtual void Stop()
            {
                if (playing)
                {
                    playing = false;
                    AudioQueueStop(queue, true);
                }
            }
        };
        
        class CoreAudioChannel : public Channel
        {
            AVAudioPlayer* player;
            
        public:
            CoreAudioChannel(const char* filename, PlayMode mode, bool loop, bool play)
            {
                NSString* string = [NSString stringWithFormat:@"%@/data/%@", [[NSBundle mainBundle] resourcePath], [NSString stringWithUTF8String:filename]];
                
                NSURL* url = [NSURL fileURLWithPath:string];
                //[string release];
                
                NSError* error;
                player = [[AVAudioPlayer alloc] initWithContentsOfURL:url error:&error];
                //[url release];
                
                if (player == nil)
                {
                    Xli::ErrorPrintLine("ERROR: Failed to create AVAudioPlayer: " + (Xli::CharString)([[error description] UTF8String]));
                    return;
                }
                
                if (loop)
                {
                    player.numberOfLoops = -1;
                }
                
                if (play)
                {
                    [player play];
                }
            }
            
            virtual ~CoreAudioChannel()
            {
                if (player == nil) return;
                [player release];
            }
            
            double GetDuration()
            {
                if (player == nil) return 0.0;
                return player.duration;
            }
            
            virtual double GetPosition()
            {   
                if (player == nil) return 0.0;
                return player.currentTime;
            }
            virtual void SetPosition(double value)
            {
                if (player == nil) return;
                player.currentTime = value;
            }
            
            virtual float GetPan()
            {
                if (player == nil) return 0.0f;
                return player.pan;
            }
            virtual void SetPan(float value)
            {
                if (player == nil) return;
                player.pan = value;
            }
            
            virtual float GetVolume()
            {
                if (player == nil) return 0.0f;
                return player.volume;
            }
            virtual void SetVolume(float value)
            {
                if (player == nil) return;
                player.volume = value;
            }
            
            virtual bool GetPause()
            {
                if (player == nil) return true;
                return !player.playing;
            }
            virtual void SetPause(bool value)
            {
                if (player == nil) return;
                if (value) [player play];
                else [player pause];
            }
            
            virtual bool GetIsPlaying()
            {
                if (player == nil) return false;
                return player.playing;
            }
            virtual bool GetIsFinished()
            {
                if (player == nil) return true;
                return false;
            }
            
            virtual void Stop()
            {
                if (player == nil) return;
                [player stop];
                [player release];
                player = nil;
            }
        };
        
        class DummyChannel : public Channel
        {
        public:
            DummyChannel()
            {
            }
            
            virtual ~DummyChannel()
            {
            }
            
            virtual double GetPosition()
            {
                return 0.0;
            }
            virtual void SetPosition(double value)
            {
            }
            
            virtual float GetPan()
            {
                return 0.0f;
            }
            virtual void SetPan(float value)
            {
            }
            
            virtual float GetVolume()
            {
                return 0.0f;
            }
            virtual void SetVolume(float value)
            {
            }
            
            virtual bool GetPause()
            {
                return false;
            }
            virtual void SetPause(bool value)
            {
            }
            
            virtual bool GetIsPlaying()
            {
                return false;
            }
            virtual bool GetIsFinished()
            {
                return true;
            }
            
            virtual void Stop()
            {
            }
        };
        
        Sound::Sound(const char* filename, PlayMode mode)
        {
            this->filename = filename;
            this->mode = mode;
            
            CoreAudioChannel* c = new CoreAudioChannel(this->filename.DataPtr(), mode, false, false);
            this->duration = c->GetDuration();
            delete c;
            
            if (mode != PlayModeStream) this->bufferedData = new AudioPacketBuffer(filename);
            else this->bufferedData = 0;
        }
        
        Sound::~Sound()
        {
            if (bufferedData) delete (AudioPacketBuffer*)bufferedData;
        }
        
        double Sound::GetDuration()
        {
            return duration;
        }
        
        Channel* Sound::Play()
        {
            if (mode != PlayModeStream) return new (uno_gc) AudioQueueChannel((AudioPacketBuffer*)bufferedData, false);
            return new CoreAudioChannel(filename.DataPtr(), mode, false, true);
        }
        
        Channel* Sound::PlayLoop()
        {
            if (mode != PlayModeStream) return new (uno_gc) AudioQueueChannel((AudioPacketBuffer*)bufferedData, true);
            return new (uno_gc) CoreAudioChannel(filename.DataPtr(), mode, true, true);
        }
    }
}
