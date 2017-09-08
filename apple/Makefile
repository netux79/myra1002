IPHONE_IP:=
PROJECTNAME:=RetroArch
APPFOLDER:=xcbuild/$(PROJECTNAME).app
INSTALLFOLDER:=$(PROJECTNAME).app

CC:=ios-clang
CPP:=ios-clang++

CFLAGS += -I./common -I./iOS -I. -I..

COMMON_FLAGS := -DIOS -DHAVE_GRIFFIN -DHAVE_MENU -DHAVE_DYNAMIC -DHAVE_OPENGL
COMMON_FLAGS += -DHAVE_FBO -DHAVE_OPENGLES -DHAVE_OPENGLES2 -DHAVE_GLSL -DINLINE=inline -DHAVE_THREADS -D__LIBRETRO__
COMMON_FLAGS += -std=gnu99 -DHAVE_COREAUDIO -DHAVE_OVERLAY -DHAVE_ZLIB -DSINC_LOWER_QUALITY -DRARCH_INTERNAL -DLSB_FIRST
COMMON_FLAGS += -DNS_BLOCK_ASSERTIONS=1 -DNDEBUG -DRARCH_MOBILE -DWANT_MINIZ -DHAVE_SCREENSHOTS -DHAVE_FILTERS_BUILTIN
COMMON_IOS_OBJCFLAGS := -fobjc-arc 

LDFLAGS += -framework GameController
LDFLAGS += -framework CoreLocation
LDFLAGS += -framework CoreMedia
LDFLAGS += -framework AVFoundation
LDFLAGS += -framework CoreVideo
LDFLAGS += -framework AudioToolbox
LDFLAGS += -framework CoreAudio
LDFLAGS += -framework UIKit
LDFLAGS += -framework Foundation
LDFLAGS += -framework CoreGraphics
LDFLAGS += -framework GLKit
LDFLAGS += -framework OpenGLES


all: $(PROJECTNAME)

OBJS+=  \
	./../griffin/griffin.o \
	./../audio/utils_neon.o \
	./../audio/sinc_neon.o \
	./common/apple_gamecontroller.o \
	./common/main.o \
	./common/RAGameView.o \
	./common/utility.o \
	./iOS/menu.o \
	./iOS/browser.o \
	./iOS/platform.o

$(PROJECTNAME): \
	./../griffin/griffin.o \
	./../audio/utils_neon.o \
	./../audio/sinc_neon.o \
	./common/apple_gamecontroller.o \
	./common/main.o \
	./common/RAGameView.o \
	./common/utility.o \
	./iOS/menu.o \
	./iOS/browser.o \
	./iOS/platform.o
	mkdir -p xcbuild
	$(CC) $(CFLAGS) $(COMMON_FLAGS) $(COMMON_IOS_OBJCFLAGS) $(LDFLAGS) $(filter %.o,$^) -o xcbuild/$@

./common/apple_gamecontroller.o: ./common/apple_gamecontroller.m
	$(CC) -c $(CFLAGS) $(COMMON_FLAGS) $(COMMON_IOS_OBJCFLAGS) $< -o $@

./common/main.o: ./common/main.m
	$(CC) -c $(CFLAGS) $(COMMON_FLAGS) $(COMMON_IOS_OBJCFLAGS) $< -o $@

./common/RAGameView.o: ./common/RAGameView.m
	$(CC) -c $(CFLAGS) $(COMMON_FLAGS) $(COMMON_IOS_OBJCFLAGS) $< -o $@

./../audio/utils_neon.o: ./../audio/utils_neon.S
	$(CC) -c $(CFLAGS) $(COMMON_FLAGS) $(COMMON_IOS_OBJCFLAGS) $< -o $@

./../griffin/griffin.o: ./../griffin/griffin.c
	$(CC) -c $(CFLAGS) $(COMMON_FLAGS) $(COMMON_IOS_OBJCFLAGS) $< -o $@

./../audio/sinc_neon.o: ./../audio/sinc_neon.S
	$(CC) -c $(CFLAGS) $(COMMON_FLAGS) $(COMMON_IOS_OBJCFLAGS) $< -o $@

./common/utility.o: ./common/utility.m
	$(CC) -c $(CFLAGS) $(COMMON_FLAGS) $(COMMON_IOS_OBJCFLAGS) $< -o $@

./iOS/menu.o: ./iOS/menu.m
	$(CC) -c $(CFLAGS) $(COMMON_FLAGS) $(COMMON_IOS_OBJCFLAGS) $< -o $@

./iOS/browser.o: ./iOS/browser.m
	$(CC) -c $(CFLAGS) $(COMMON_FLAGS) $(COMMON_IOS_OBJCFLAGS) $< -o $@

./iOS/platform.o: ./iOS/platform.m
	$(CC) -c $(CFLAGS) $(COMMON_FLAGS) $(COMMON_IOS_OBJCFLAGS) $< -o $@

INFOPLIST:=$(SRCROOT)/iOS/RetroArch-Info.plist

RESOURCES += \
	./Assets/../../android/phoenix/res/drawable-xhdpi/ic_dir.png \
	./Assets/../../android/phoenix/res/drawable-xhdpi/ic_file.png \
	./Assets/../../media/overlays \
	./Assets/Default-568h@2x.png \
	./Assets/Default.png \
	./Assets/Default@2x.png \
	./Assets/Icon-72.png \
	./Assets/Icon.png \
	./Assets/PauseIndicatorView.xib \
	./Assets/ic_pause.png \
	./Assets/modules \
	./RetroArch/iOS/en.lproj

dist: $(PROJECTNAME)
	mkdir -p $(APPFOLDER)
	cp -r $(RESOURCES) $(APPFOLDER)
	cp $(INFOPLIST) $(APPFOLDER)/Info.plist
	cp xcbuild/$(PROJECTNAME) $(APPFOLDER)
	sed -i 's|$${EXECUTABLE_NAME}|RetroArch|g' $(APPFOLDER)/Info.plist
	sed -i 's|$${PRODUCT_NAME}|RetroArch|g' $(APPFOLDER)/Info.plist
	sed -i 's|$${PRODUCT_NAME:identifier}|retroarch|g' $(APPFOLDER)/Info.plist
	sed -i 's|$${PRODUCT_NAME:rfc1034identifier}|retroarch|g' $(APPFOLDER)/Info.plist
	find $(APPFOLDER) -name \*.png|xargs ios-pngcrush -c
	find $(APPFOLDER) -name \*.plist|xargs ios-plutil -c
	find $(APPFOLDER) -name \*.strings|xargs ios-plutil -c

langs:
	ios-genLocalization

install: dist
ifeq ($(IPHONE_IP),)
	echo "Please set IPHONE_IP"
else
	ssh root@$(IPHONE_IP) 'rm -fr /Applications/$(INSTALLFOLDER)'
	scp -r $(APPFOLDER) root@$(IPHONE_IP):/Applications/$(INSTALLFOLDER)
	echo "Application $(INSTALLFOLDER) installed"
	ssh mobile@$(IPHONE_IP) 'uicache'
endif

uninstall:
ifeq ($(IPHONE_IP),)
	echo "Please set IPHONE_IP"
else
	ssh root@$(IPHONE_IP) 'rm -fr /Applications/$(INSTALLFOLDER)'
	echo "Application $(INSTALLFOLDER) uninstalled"
endif

clean:
	find . -name \*.o|xargs rm -rf
	rm -rf xcbuild
