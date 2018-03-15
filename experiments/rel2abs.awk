#!/usr/bin/awk -f

{
    abs_time = abs_time + $2
    printf "%s %10f\n", $1, abs_time
}
