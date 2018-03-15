#!/usr/bin/awk -f

BEGIN {
    gear = ""
    rotation = "E"
    velocity = "G"
}

{
    currGear = substr($1, 1, 1)
    currRotation = substr($1, 2, 1)
    currVelocity = substr($1, 3, 1)
    if (gear != currGear) {
        print currGear, $2
    } else if (rotation != currRotation) {
        print currRotation, $2
    } else if (velocity != currVelocity) {
        print currVelocity, $2
    }
    gear = currGear
    rotation = currRotation
    velocity = currVelocity
}
