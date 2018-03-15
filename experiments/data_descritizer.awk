#!/usr/bin/awk -f

BEGIN {
    velocity_threshold = 100
    rotation_threshold = 2500
}

## LOG FORMAT
# ./data_descritizer.awk /tmp/Experiment3_AT-172800.tsv | tac | uniq -w 3 | tac 
# input: time rotation velocity gear
# output: gear (ABCD) rotation (EF) velocity (GH) time 
{
    if ($2 >= rotation_threshold) {
        rotation = "F";
    } else {
        rotation = "E";
    }
    if ($3 >= velocity_threshold) {
        velocity = "H";
    } else {
        velocity = "G";
    }
    switch($4) {
        case "1":
            gear = "A";
            break;
        case "2":
            gear = "B";
            break;
        case "3":
            gear = "C";
            break;
        case "4":
            gear = "D";
            break;
    }
    printf "%c%c%c\t%s\n", gear, rotation, velocity, $1
}

END{
}
