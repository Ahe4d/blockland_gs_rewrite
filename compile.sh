i686-w64-mingw32-gcc -shared "./dllmain.cpp" "./main.cpp" "./client_audio.cpp" "./Torque.cpp" -g \
-o "Rewrite.dll" \
-I./include \
-static \
-static-libgcc \
-static-libstdc++ \
-lstdc++ \
-lpsapi \
-lpthread \
-w 

mv Rewrite.dll ~/BlocklandPortable/modules/OurRewrite.dll

