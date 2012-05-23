#!/bin/sh

# -*- tcl -*- \
exec tclsh "$0" "$@"
package require Expect

set port 9999

# needed! for TET
if {$argc == 1} {
  set argv [split $argv ,]
  set argc [llength $argv]
}

if { $argc != 3 } {
  puts "Invalid Usage! \n./transaction.tcl <type> <size> <reps>"
  exit 0
}

set port $port
set type [lindex $argv 0]
set size [lindex $argv 1]
set reps [lindex $argv 2]


#####
# Generate strings
#
# Exchange is five strings, randomly generated
# 
set range "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#%^&*<>"
set x ""
set c 0
for {set a 0} {$a < $reps} {incr a} {
  while {$c < $size} {
    set r [expr round(rand()*70)]
    append x [string index $range $r]
    incr c
  }
  set strings($a) $x
  set x ""
  set c 0
}
########
# Open connection
#
# Types are: SSL Decryption to IP (SSLIP), SSL Decryption to Serial (SSLSerial), Serial to IP (Serial), IP to Serial (IP)
# Dial to IP (Dial), IP to IP (IPUP)
#
switch $type {
  "IPIP" {
    set type "IP to IP"
    puts $type
    spawn socket -qvls $port
    expect {
      "listening on port $port" { puts "Listening for POSLynx" }
      "Address already in use" { puts "No connection possible. Waiting."; sleep 60; spawn socket -qvls $port; exp_continue }
    }
    set reception $spawn_id
    spawn socket -qv 10.0.3.77 9999
    expect {
      "connected to 10.0.3.77 port 9999" { puts "Connection made to POSLynx" }
    }
    set insertion $spawn_id
    expect {
      -i $reception "connection from 10.0.3.77" { puts "Connection Established" }
    }
  }
  "SSLIP" { 
    set type "SSL to IP" 
    spawn socket -qvls 9998
    expect {
      "listening on port 9998" { puts "Listening for POSLynx" }
      "Address already in use" { puts "No connection possible. Waiting."; sleep 60; spawn socket -qvls $port; exp_continue }
    }
    set reception $spawn_id
    spawn telnet -zssl 10.0.3.77 9998
    expect {
      "Escape character is '^]'." { puts "Connection made to POSLynx" }
    }
    set insertion $spawn_id
    expect {
      -i $reception "connection from 10.0.3.77" { puts "Connection Established" }
    }
  }
  "SSLSerial" { 
    set type "SSL to Serial" 
    spawn tip -8n1 -s9600 /dev/ttyS1
    expect "Connected"
    exp_send "ATDT1\r"
    set reception $spawn_id
    sleep 1
    spawn telnet -z ssl 10.0.3.77 $port
    expect {
      "Escape character" { set insertion $spawn_id }
      "telnet: Unable to connect to remote host: Connection refused" { set success "ERROR: POSLynx not listening for connection"; set status 1 }
    }
    expect {
      -i $reception
      "CONNECT 9600" { }
    }
  }
  "Serial" { 
    set type "Serial to IP"
#    spawn socket -qvls $port
#    expect "listening on port $port"
#    set reception $spawn_id 
#    sleep 4
    spawn tip -8n1 -s9600 /dev/ttyS$port
#    expect "Connected."
#    exp_send "ATDT1\r"
#    expect "CONNECT"
    set insertion $spawn_id
    set reception $spawn_id
#    expect -i $reception "connection from 10.0.3.77"
  }
  "Dial" {
    set type "Dial to IP"
    spawn socket -qvls $port
    expect "listening on port $port"
    set reception $spawn_id
    sleep 4
    spawn tip -r8n1 -s9600 /dev/ttyS0
    expect "Connected."
    exp_send "ATDT1\r"
    expect "CONNECT"
    set insertion $spawn_id
    expect -i $reception "connection from 10.0.3.77"
    exp_send -i $reception "\n"
  }
  "IP" { 
    set type "IP to Serial"
    spawn socket 10.0.3.77 $port
    set insertion $spawn_id
    spawn tip -8n1 -s9600 /dev/ttyS1
    expect "Escape character"
    set reception $spawn_id
  }
}

####
# Send strings; expecting responses
#
set success "Error - No Result"
set successrate 0
set failed ""
expect -i $reception -re ".*"

set timer 60
puts "*** $timer second timeout ***"
sleep 4
for {set a 0} {$a < $reps} {incr a} {
  exp_send -i $insertion "$strings($a)\r\n"
  expect {
    -i $reception
    -timeout $timer
    -re {(.+)[\r\n]+} {
      set x [string length [string trim $expect_out(1,string)]]
      if {$x < 2} { exp_continue } else {
        set i [string trim $strings($a)]
        set r [string trim $expect_out(1,string)]
        if {$i eq $r} { incr successrate }
        puts "=-=-=-=-=-=-=-=-="
        puts "Match I: '$i'"
        puts "Match R: '$r' (length $x)"
        if {$i ne $r} { puts "Failed to Match"; append failed " $a" } 
        puts "=-=-=-=-=-=-=-=-="
      }
    }
    timeout {
        set i [string trim $strings($a)]
        set r [string trim $expect_out(buffer)]
        set x [string length $r]
        if {$i eq $r} { incr successrate }
        puts "=-=-=-=-=-=-=-=-="
        puts "Match I: '$i'"
        puts "Match R: '$r' (length $x)"
        if {$i ne $r} { puts "Failed to Match"; append failed " $a" }
        puts "=-=-=-=-=-=-=-=-="
    }
  }
  sleep [expr $size/1024 + 1]
  expect -i $reception -re ".*"
}
exp_close -i $reception
exp_close -i $insertion
exp_wait

set failed [string trim $failed]
if {$successrate == $reps} {
  set success "Successful"
  set details "$successrate of $reps messages of length $size received with no errors"
} elseif {$successrate == 0} {
  set success "Failure"
  set details "0 of $reps messages of length $size received with no errors"
} else {
  set success "Indeterminate"
  set details "$successrate of $reps messages of length $size received with no errors"
  append details "<br/>Failed on message(s) $failed"
}

set date [clock format [clock seconds] -format "%m%d%y"]
set time [clock format [clock seconds] -format "%H%M%S"]

set Trans "$type Data Passthrough"
puts $Trans
puts "Result: $success"
puts "$details"
