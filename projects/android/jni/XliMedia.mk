include $(CLEAR_VARS)
LOCAL_MODULE    := XliMedia

LOCAL_SRC_FILES := \
    ../../../src/audio/opensles/SlesSimpleAudio.cpp \
	../../../src/media/3rdparty/tinyxml/tinystr.cpp \
	../../../src/media/3rdparty/tinyxml/tinyxml.cpp \
	../../../src/media/3rdparty/tinyxml/tinyxmlerror.cpp \
	../../../src/media/3rdparty/tinyxml/tinyxmlparser.cpp \
	../../../src/media/3rdparty/json_parser/JSON_parser.c \
	../../../src/media/3rdparty/unzip/ioapi.c \
	../../../src/media/3rdparty/unzip/unzip.c \
	../../../src/media/3rdparty/jpeg/jcapimin.c \
	../../../src/media/3rdparty/jpeg/jcapistd.c \
	../../../src/media/3rdparty/jpeg/jccoefct.c \
	../../../src/media/3rdparty/jpeg/jccolor.c \
	../../../src/media/3rdparty/jpeg/jcdctmgr.c \
	../../../src/media/3rdparty/jpeg/jchuff.c \
	../../../src/media/3rdparty/jpeg/jcinit.c \
	../../../src/media/3rdparty/jpeg/jcmainct.c \
	../../../src/media/3rdparty/jpeg/jcmarker.c \
	../../../src/media/3rdparty/jpeg/jcmaster.c \
	../../../src/media/3rdparty/jpeg/jcomapi.c \
	../../../src/media/3rdparty/jpeg/jcparam.c \
	../../../src/media/3rdparty/jpeg/jcphuff.c \
	../../../src/media/3rdparty/jpeg/jcprepct.c \
	../../../src/media/3rdparty/jpeg/jcsample.c \
	../../../src/media/3rdparty/jpeg/jctrans.c \
	../../../src/media/3rdparty/jpeg/jdapimin.c \
	../../../src/media/3rdparty/jpeg/jdapistd.c \
	../../../src/media/3rdparty/jpeg/jdatadst.c \
	../../../src/media/3rdparty/jpeg/jdatasrc.c \
	../../../src/media/3rdparty/jpeg/jdcoefct.c \
	../../../src/media/3rdparty/jpeg/jdcolor.c \
	../../../src/media/3rdparty/jpeg/jddctmgr.c \
	../../../src/media/3rdparty/jpeg/jdhuff.c \
	../../../src/media/3rdparty/jpeg/jdinput.c \
	../../../src/media/3rdparty/jpeg/jdmainct.c \
	../../../src/media/3rdparty/jpeg/jdmarker.c \
	../../../src/media/3rdparty/jpeg/jdmaster.c \
	../../../src/media/3rdparty/jpeg/jdmerge.c \
	../../../src/media/3rdparty/jpeg/jdphuff.c \
	../../../src/media/3rdparty/jpeg/jdpostct.c \
	../../../src/media/3rdparty/jpeg/jdsample.c \
	../../../src/media/3rdparty/jpeg/jdtrans.c \
	../../../src/media/3rdparty/jpeg/jerror.c \
	../../../src/media/3rdparty/jpeg/jfdctflt.c \
	../../../src/media/3rdparty/jpeg/jfdctfst.c \
	../../../src/media/3rdparty/jpeg/jfdctint.c \
	../../../src/media/3rdparty/jpeg/jidctflt.c \
	../../../src/media/3rdparty/jpeg/jidctfst.c \
	../../../src/media/3rdparty/jpeg/jidctint.c \
	../../../src/media/3rdparty/jpeg/jidctred.c \
	../../../src/media/3rdparty/jpeg/jmemmgr.c \
	../../../src/media/3rdparty/jpeg/jmemnobs.c \
	../../../src/media/3rdparty/jpeg/jquant1.c \
	../../../src/media/3rdparty/jpeg/jquant2.c \
	../../../src/media/3rdparty/jpeg/jutils.c \
	../../../src/media/3rdparty/png/png.c \
	../../../src/media/3rdparty/png/pngerror.c \
	../../../src/media/3rdparty/png/pngget.c \
	../../../src/media/3rdparty/png/pngmem.c \
	../../../src/media/3rdparty/png/pngpread.c \
	../../../src/media/3rdparty/png/pngread.c \
	../../../src/media/3rdparty/png/pngrio.c \
	../../../src/media/3rdparty/png/pngrtran.c \
	../../../src/media/3rdparty/png/pngrutil.c \
	../../../src/media/3rdparty/png/pngset.c \
	../../../src/media/3rdparty/png/pngtrans.c \
	../../../src/media/3rdparty/png/pngwio.c \
	../../../src/media/3rdparty/png/pngwrite.c \
	../../../src/media/3rdparty/png/pngwtran.c \
	../../../src/media/3rdparty/png/pngwutil.c \
	../../../src/media/3rdparty/freetype/src/base/ftbbox.c \
	../../../src/media/3rdparty/freetype/src/base/ftbitmap.c \
	../../../src/media/3rdparty/freetype/src/base/ftglyph.c \
	../../../src/media/3rdparty/freetype/src/base/ftstroke.c \
	../../../src/media/3rdparty/freetype/src/base/ftxf86.c \
	../../../src/media/3rdparty/freetype/src/base/ftbase.c \
	../../../src/media/3rdparty/freetype/src/base/ftsystem.c \
	../../../src/media/3rdparty/freetype/src/base/ftinit.c \
	../../../src/media/3rdparty/freetype/src/base/ftgasp.c \
	../../../src/media/3rdparty/freetype/src/raster/raster.c \
	../../../src/media/3rdparty/freetype/src/sfnt/sfnt.c \
	../../../src/media/3rdparty/freetype/src/smooth/smooth.c \
	../../../src/media/3rdparty/freetype/src/autofit/autofit.c \
	../../../src/media/3rdparty/freetype/src/truetype/truetype.c \
	../../../src/media/3rdparty/freetype/src/cff/cff.c \
	../../../src/media/3rdparty/freetype/src/psnames/psnames.c \
	../../../src/media/3rdparty/freetype/src/pshinter/pshinter.c \
	../../../src/media/Bitmap.cpp \
	../../../src/media/CompressedImage.cpp \
	../../../src/media/FontFace.cpp \
	../../../src/media/Format.cpp \
	../../../src/media/FormattedTextWriter.cpp \
	../../../src/media/GZip.cpp \
	../../../src/media/ImageReader.cpp \
	../../../src/media/ImageWriter.cpp \
	../../../src/media/JpegReader.cpp \
	../../../src/media/JpegWriter.cpp \
	../../../src/media/Json.cpp \
	../../../src/media/Ktx.cpp \
	../../../src/media/PngReader.cpp \
	../../../src/media/PngWriter.cpp \
	../../../src/media/Texture.cpp \
	../../../src/media/Value.cpp \
	../../../src/media/Xml.cpp \
	../../../src/media/Zip.cpp \

LOCAL_C_INCLUDES := \
	../../include \
	../../src/media/3rdparty/freetype/include \
	../../src/media/3rdparty/jpeg \
	../../src/media/3rdparty/png \
	../../src/media/3rdparty/tinyxml \ 

LOCAL_CFLAGS   := -fexceptions -fno-rtti -fPIC -DPIC -O3
LOCAL_CFLAGS   += "-DDARWIN_NO_CARBON"
LOCAL_CFLAGS   += "-DFT2_BUILD_LIBRARY"
LOCAL_LDLIBS   := -lz -llog -lOpenSLES -landroid

LOCAL_SHARED_LIBRARIES := Xli

include $(BUILD_SHARED_LIBRARY)