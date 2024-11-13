find music2/lib/*.a prx/usr/local/pspdev/psp/sdk/lib/*.a -exec bash -c "echo {} ; nm {} | grep --color $1" \;
