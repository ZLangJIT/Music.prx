find music1/lib/*.a -exec bash -c "echo {} ; nm {} | grep --color $1" \;
find music1/*.c music1/*.h -exec bash -c "echo {} ; grep -n --color $1 {}" \;
