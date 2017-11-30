use <threads.scad>

// Constants
OFF_SIZE = 6400 / 1024;
STOP_SIZE = 2400 / 1024;
MARK_DEPTH = 0.2;

// Panel
difference() {
    // The panel itself
    color("white") cube(size=[170, 52, 1.5], center=true);

    // Center features on the Z axis so they cut all the way through
    translate([0, 0, -1.5]) {
        // Mounting screw holes
        translate([-60, 0]) metric_thread(3, 0.5, 3);
        translate([60, 0]) metric_thread(3, 0.5, 3);

        // Groove for slider
        hull() {
            translate([-54, 0]) cylinder(h=3, d=3, $fn=50);
            translate([54, 0]) cylinder(h=3, d=3, $fn=50);
        }
    }

    // Markings for stops
    color("black")
    translate([-50 + OFF_SIZE + STOP_SIZE / 2, 0, 0.75])
    {
        translate([0, 1.5]) stopSequence();
        translate([0, -1.5]) stopSequence(flip=true);
    }

    // OFF markings
    color("black")  {
        translate([-50 + OFF_SIZE / 2, 10.75, 0.75 - MARK_DEPTH])
        linear_extrude(height=MARK_DEPTH)
        text(
            text="OFF",
            size=2,
            font="Helvetica Neue:style=Condensed Bold",
            halign="center",
            valign="bottom"
        );

        translate([-50 + OFF_SIZE / 2, -10.75, 0.75 - MARK_DEPTH])
        linear_extrude(height=MARK_DEPTH)
        text(
            text="OFF",
            size=2,
            font="Helvetica Neue:style=Condensed Bold",
            halign="center",
            valign="top"
        );
    }
}

module stopSequence(flip=false) {
    // Bold numbers (zero and scale changes)
    for (params=[[0, "0"], [6, "3"], [33, "30"], [39, "60"]]) {
        translate([params[0] * STOP_SIZE, 0]) {
            stopMark(length=8, text=params[1], bold=true, flip=flip);
        }
    }

    // Marked lines
    for (params=[[2, "1"], [4, "2"], [8, "5"], [13, "10"],
                 [18, "15"], [23, "20"], [28, "25"],
                 [36, "45"]])
    {
        translate([params[0] * STOP_SIZE, 0]) {
            stopMark(length=7, text=params[1], flip=flip);
        }
    }
    
    // Remaining lines
    for (offset=[1, 3, 5, 7, 9, 10, 11, 12, 14, 15, 16, 17,
                 19, 20, 21, 22, 24, 25, 26, 27, 29, 30, 31,
                 32, 34, 35, 37, 38])
    {
        length = offset < 6 ? 2 : (offset > 33 ? 5 : 3.5);

        translate([offset * STOP_SIZE, 0]) {
            stopMark(length=length, flip=flip);
        }
    }
}

module stopMark(length=1, text="", bold=false, flip=false) {
    width = bold ? 0.4 : 0.2;
    lineOffset = flip ? -8 : (8 - length);
    textOffset = flip ? -9 : 9;
    textValign = flip ? "top" : "bottom";

    translate([-width / 2, lineOffset, -MARK_DEPTH]) {
        cube(size=[width, length, MARK_DEPTH]);
    }

    if (text != "") {
        translate([0, textOffset, -MARK_DEPTH])
        linear_extrude(height=MARK_DEPTH)
        {
            text(
                text=text,
                size=2.5,
                font=str("Helvetica Neue", bold ? ":style=Bold" : ""),
                halign="center",
                valign=textValign
            );
        }
    }
}