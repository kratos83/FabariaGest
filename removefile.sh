#!/bin/sh

echo "Rimozione file cpp_parameters"
find . -type f -name '*.cpp_parameters' -delete
echo "Rimozione file cbp"
find . -type f -name '*.cbp' -delete
echo "Rimozione file depends"
find . -type f -name '*.depends' -delete
echo "Rimozione file CMakeCache"
find . -type f -name 'CMakeCache.txt' -delete
echo "Rimozione file ui_*"
find . -type f -name 'ui_*' -delete
echo "Rimozione file Makefile"
find . -type f -name 'Makefile' -delete
echo "Rimozione file CMakeFiles"
find . -type d -name 'CMakeFiles' -exec rm -rf {} \;
echo "Rimozione file CPackConfig.cmake"
find . -type f -name 'CPackConfig.cmake' -delete
echo "Rimozione file CPackSourceConfig.cmake"
find . -type f -name 'CPackSourceConfig.cmake' -delete
echo "Rimozione file compile_commands.json"
find . -type f -name 'compile_commands.json' -delete
echo "Rimozione file parameter"
find . -type f -name '*.cxx_parameters' -delete
echo "Rimozione file moc"
find . -type f -name 'moc_*' -delete
