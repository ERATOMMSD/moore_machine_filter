#!/usr/bin/awk -f

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
