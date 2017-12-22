i686-w64-mingw32-gcc -shared "./dllmain.cpp" "./Torque.cpp" -g \
-o "Rewrite.dll" \
-I./include \
-static \
-static-libgcc \
-static-libstdc++ \
-lstdc++ \
-lpsapi \
-lpthread \
-w 

cp Rewrite.dll ~/BlocklandPortable/modules/OurRewrite.dll

