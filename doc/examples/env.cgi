#! /usr/bin/env pez

hex

: each-env ( word -- )
	environ
	begin dup @ while
		2dup @ swap execute
		cell-size +
	repeat 2drop ;

: print-env-var ( str -- )
	puts ;


"Content-type: text/plain\r\n\r\n" print
"Hello, world!" puts
"Env pointer:  " print environ . cr cr
' print-env-var each-env
