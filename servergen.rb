#!/usr/bin/env ruby

prev = nil

IO.read('server.calls').lines do |line|

  line =~ /linux_socketcall\((\d+),{ 100, (\w+) }\)/

  case $1.to_i
    when  9 # SYS_SEND
      puts "SND_AND_LOG(__func__, cfd, buf, #{$2.to_i(16)}); if (nbytes == -1) { perror(\"[server] send\"); assert(errno != EAGAIN); exit(0); }"
    when 10 # SYS_RECV
      #~ if !prev || prev == 9
        #~ puts "usleep(SERVER_SLEEP_INTERVAL);"
      #~ end
      #~ puts "RCV_AND_LOG(__func__, cfd, buf, #{$2}); if (nbytes == -1) { perror(\"[server] recv\"); assert(errno != EAGAIN); exit(0); }"
  end

  prev = $1.to_i
end
