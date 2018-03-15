#!/usr/bin/awk -f

{
    abs_time = abs_time + $2
    $2 = abs_time
}
$1 != "_" && truncating {
    print last
    truncating = 0
}
$1 != "_"
$1 == "_" && !truncating
$1 == "_" {
    truncating = 1
    last = $0
}
