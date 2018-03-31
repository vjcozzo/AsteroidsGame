SHELL=/bin/bash
CXX=g++
CXXFLAGS=-Wall -Wextra -std=c++11 # -Werror
# Flags for the RELEASE branch (no debugging symbols)
REL_DYN=-O2 -fPIC
REL_STATIC=-O2 -static -fPIC
# Flags for the DEBUG branch (debugging symbols active)
DEBUG_DYN=-g -O0 -fPIC
DEBUG_STC=-g -O0 -static -fPIC
# Flags for the OPTIMIZE_SIZE branch
SIZE_DYN=-Os -fPIC
SIZE_STC=-Os -static -fPIC

# Library flags from allegro 5
# TODO -- re-build the allegro 5 static libraries. They have not been working right lately.
LIBS=-lallegro_primitives -lallegro_main -lallegro_acodec -lallegro_dialog -lallegro_ttf -lallegro_audio -lallegro_font -lallegro_image -lallegro
#STATIC_LIBS=-I/usr/local/include -L/usr/local/lib -lallegro_primitives-static -lallegro_main-static -lallegro_acodec-static -lFLAC -logg -lvorbisfile -lvorbis -logg -lallegro_dialog-static -lgtk-x11-2.0 -lgdk-x11-2.0 -lpangocairo-1.0 -latk-1.0 -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lpangoft2-1.0 -lpango-1.0 -lgobject-2.0 -lglib-2.0 -lfontconfig -lfreetype -lgthread-2.0 -lglib-2.0 -lallegro_ttf-static -lfreetype -lallegro_audio-static -lpulse-simple -lpulse -lasound -lopenal -lallegro_font-static -lallegro_image-static -lpng -lz -ljpeg -lwebp -lallegro-static -lm -lpthread -lSM -lICE -lX11 -lXext -lXcursor -lXpm -lXi -lXinerama -lXrandr -lGLU -lGL
#STATIC_LIBS=-I/usr/local/include -L/usr/local/lib -lallegro_color-static -lallegro_main-static -lallegro_primitives-static -lallegro_ttf-static -lallegro_font-static -lallegro-static
STATIC_LIBS=-I/usr/local/include -L/usr/local/lib -lallegro_color-static -lallegro_main-static -lallegro_primitives-static -lallegro_ttf-static -lallegro_font-static -lallegro_image-static -lallegro-static
SAVED_DATE=`/bin/date +%F`
EXEC=oo-tmb game-loop
SHARED_DIR=dynamic-libs/

# The following line is very specific to my machine.
# DEVELOPERS - Please change it when running on your machine!
# In the future - also have it saved to an external drive  for security.
BACKUP_DIR=/home/chocorho/git/chocorho/allegro-game-release

execs: $(EXEC) # major executable files, then make clean
#all: game-loop oo-tmb set-config clean release backups
all: game-loop release backups
#objects: game-loop oo-tmb set-config release backups# NOTE: NO `clean` !

.PHONY: clean
clean:
	@echo "Cleaning up all object files..."
	@rm -f *.o bin/*.o
	@echo "Finished cleaning."

# initializes directory structure... should only be called once, if at all.
#init:
#	@mkdir bin/ bin/dyn-pie/ bin/dynamic bin/static/ bin/libstatic
#	@mkdir bin/dyn-pie/release bin/dyn-pie/debug bin/dyn-pie/opt_size
#	@mkdir bin/dynamic/release bin/dynamic/debug bin/dynamic/opt_size
#	@mkdir bin/static/release bin/static/debug bin/static/opt_size
#	@mkdir bin/libstatic/release bin/libstatic/debug bin/libstatic/opt_size

game-loop: game-loop.o
	${CXX} -o bin/dyn-pie/release/game-loop bin/dyn-pie/release/game-loop-rel.cpp.o -L${SHARED_DIR} -pie -fPIE ${LIBS}
	${CXX} -o bin/dyn-pie/debug/game-loop bin/dyn-pie/debug/game-loop-debug.cpp.o -L$(SHARED_DIR) -pie -fPIE ${LIBS}
	${CXX} -o bin/dyn-pie/opt_size/game-loop bin/dyn-pie/opt_size/game-loop.cpp.o -L$(SHARED_DIR) -pie -fPIE ${LIBS}
	${CXX} -o bin/dynamic/release/game-loop bin/dyn-pie/release/game-loop-rel.cpp.o -L${SHARED_DIR} ${LIBS} # no pie
	${CXX} -o bin/dynamic/debug/game-loop bin/dyn-pie/debug/game-loop-debug.cpp.o -L$(SHARED_DIR) ${LIBS} # no pie
	${CXX} -o bin/dynamic/opt_size/game-loop bin/dyn-pie/opt_size/game-loop.cpp.o -L$(SHARED_DIR) ${LIBS} # no pie
	${CXX} -o bin/static/release/game-loop bin/static/release/game-loop.cpp.o ${STATIC_LIBS}
	${CXX} -o bin/static/debug/game-loop bin/static/debug/game-loop.cpp.o ${STATIC_LIBS}
	${CXX} -o bin/static/opt_size/game-loop bin/static/opt_size/game-loop.cpp.o ${STATIC_LIBS}
	${CXX} -o bin/libstd-static/release/game-loop bin/static/release/game-loop.cpp.o ${STATIC_LIBS} -static-libstdc++
	${CXX} -o bin/libstd-static/debug/game-loop bin/static/debug/game-loop.cpp.o ${STATIC_LIBS} -static-libstdc++
	${CXX} -o bin/libstd-static/opt_size/game-loop bin/static/opt_size/game-loop.cpp.o ${STATIC_LIBS} -static-libstdc++

game-loop.o: src/game-loop.hpp
	${CXX} -c -o bin/dyn-pie/release/game-loop-rel.cpp.o src/game-loop.cpp ${CXXFLAGS} ${REL_DYN} -pie -fPIE
	${CXX} -c -o bin/dyn-pie/debug/game-loop-debug.cpp.o src/game-loop.cpp ${CXXFLAGS} ${DEBUG_DYN} -pie -fPIE
	${CXX} -c -o bin/dyn-pie/opt_size/game-loop.cpp.o src/game-loop.cpp ${CXXFLAGS} ${SIZE_DYN} -pie -fPIE
	${CXX} -c -o bin/dynamic/release/game-loop-rel.cpp.o src/game-loop.cpp ${CXXFLAGS} ${REL_DYN}
	${CXX} -c -o bin/dynamic/debug/game-loop-debug.cpp.o src/game-loop.cpp ${CXXFLAGS} ${DEBUG_DYN}
	${CXX} -c -o bin/dynamic/opt_size/game-loop.cpp.o src/game-loop.cpp ${CXXFLAGS} ${SIZE_DYN}
	${CXX} -c -o bin/static/release/game-loop.cpp.o src/game-loop.cpp ${CXXFLAGS} ${REL_STATIC}
	${CXX} -c -o bin/static/debug/game-loop.cpp.o src/game-loop.cpp ${CXXFLAGS} ${DEBUG_STC}
	${CXX} -c -o bin/static/opt_size/game-loop.cpp.o src/game-loop.cpp ${CXXFLAGS} ${SIZE_STC}

release: gz_release bzip2_release
backups: gz_backup bzip2_backup

emergency:
	tar -czf allegro-$(SAVED_DATE)-emergency.tar.gz *
	@cp -p allegro-$(SAVED_DATE)-emergency.tar.gz -t $(BACKUP_DIR) && echo "Success" || echo "ERR: Please be sure to copy the tarball (in parent directory) to the proper location."
	@sha512sum allegro-$(SAVED_DATE)-emergency.tar.gz | tee $(BACKUP_DIR)/$(SAVED_DATE).DIGEST.asc
	@mv allegro-$(SAVED_DATE)-emergency.tar.gz -t tarball/

gz_release:
	@echo "Creating release... "
	tar -czf allegro-$(SAVED_DATE)-release.tar.gz bin/static/release/* bin/libstd-static/release/* bin/static/opt_size/* bin/libstd-static/opt_size/* res/* for-static-release-dynamic-libs/* static-libs/*
	@cp -p allegro-$(SAVED_DATE)-release.tar.gz -t $(BACKUP_DIR) && echo "Success" || echo "ERR: Please be sure to copy the tarball (in parent directory) to the proper location."
	@sha512sum allegro-$(SAVED_DATE)-release.tar.gz | tee $(BACKUP_DIR)/$(SAVED_DATE).DIGEST.asc
	@mv allegro-$(SAVED_DATE)-release.tar.gz -t tarball/

gz_backup:
	@echo "Creating backups... "
	tar -czf allegro-$(SAVED_DATE)-dev.tar.gz src/* res/* dynamic-libs/* static-libs/* for-static-release-dynamic-libs/* client-Makefiles/Makefile .*.sh # bin/*
	@cp -p allegro-$(SAVED_DATE)-dev.tar.gz -t $(BACKUP_DIR) && echo "Success" || echo "ERR: Please be sure to copy the tarball (in parent directory) to the proper location."
	@sha512sum allegro-$(SAVED_DATE)-dev.tar.gz | tee $(BACKUP_DIR)/$(SAVED_DATE).DIGEST.asc
	@mv allegro-$(SAVED_DATE)-dev.tar.gz -t tarball/

bzip2_release:
	tar -cjf allegro-$(SAVED_DATE)-release.tar.bz2 bin/static/release/* bin/libstd-static/release/* bin/static/opt_size/* bin/libstd-static/opt_size/* res/* for-static-release-dynamic-libs/* static-libs/*
	@cp -p allegro-$(SAVED_DATE)-release.tar.bz2 -t $(BACKUP_DIR) && echo "Success" || echo "ERR: Please be sure to copy the tarball (in parent directory) to the proper location."
	@sha512sum allegro-$(SAVED_DATE)-release.tar.bz2 | tee $(BACKUP_DIR)/$(SAVED_DATE).DIGEST.asc
	@mv allegro-$(SAVED_DATE)-release.tar.bz2 -t tarball/

bzip2_backup:
	tar -cjf allegro-$(SAVED_DATE)-dev.tar.bz2 src/* res/* dynamic-libs/* static-libs/* for-static-release-dynamic-libs/* client-Makefiles/Makefile .*.sh # bin/*
	@cp -p allegro-$(SAVED_DATE)-dev.tar.bz2 -t $(BACKUP_DIR) && echo "Success" || echo "ERR: Please be sure to copy the tarball (in parent directory) to the proper location."
	@sha512sum allegro-$(SAVED_DATE)-dev.tar.bz2 | tee -a $(BACKUP_DIR)/$(SAVED_DATE).DIGEST.asc
	@mv allegro-$(SAVED_DATE)-dev.tar.bz2 -t tarball/

devlog:
	@echo "Creating today's developer log... "
	touch logs/dev-log-$(SAVED_DATE).txt

devlog-send:
	@echo "Copying developer logs... "
	@cp -p dev-log-$(SAVED_DATE).txt -t $(BACKUP_DIR) && echo "Success" || echo "ERR: Failed to copy the developer logs to backup dir."

