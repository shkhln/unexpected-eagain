#!/usr/bin/env ruby

prev = nil

IO.read('client.calls').lines do |line|

  if line =~ /linux_sendto\(0x2d,\w+,(\w+),0x4000,0x0,0x0\)/
    puts "SND_AND_LOG(__func__, fd, buf, #{$1}); if (nbytes == -1) { perror(\"[client] send\"); assert(errno != EAGAIN); exit(0); }"
    prev = :snd
  end

  if line =~ /linux_recvfrom\(0x2d,\w+,(\w+),0x0,0x0,0x0\)/
    if !prev || prev == :snd
      puts "usleep(CLIENT_SLEEP_INTERVAL);"
    end
    puts "RCV_AND_LOG(__func__, fd, buf, #{$1}); if (nbytes == -1) { perror(\"[client] recv\"); assert(errno != EAGAIN); exit(0); }"
    prev = :rcv
  end
end
