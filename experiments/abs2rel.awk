#!/usr/bin/awk -f

{
    printf "%s %10f\n", $1, $2 - last_time
    last_time = $2
}
