// Constants
OFF_SIZE = 6400 / 1024;
STOP_SIZE = 2400 / 1024;

// Main panel
difference() {
    // The panel itself
    color("white")
    square(size=[170, 52], center=true);

    // Mounting screw holes
    translate([-60, 0]) circle(d=3, $fn=50);
    translate([60, 0]) circle(d=3, $fn=50);

    // Groove for slider
    hull() {
        translate([-54, 0]) circle(d=3, $fn=50);
        translate([54, 0]) circle(d=3, $fn=50);
    }

    // Markings for stops
    translate([-50 + OFF_SIZE + STOP_SIZE / 2, 0])
    {
        translate([0, 3.5]) stopSequence();
        translate([0, -3.5]) stopSequence(flip=true);
    }

    // OFF markings
    translate([-50, 4]) offMarks();
    translate([-50, -11]) offMarks();
}

module stopSequence(flip=false) {
    // Bold numbers (zero and scale changes)
    for (params=[[0, "0"], [6, "3"], [33, "30"], [39, "60"]]) {
        translate([params[0] * STOP_SIZE, 0])
        stopMark(length=9, text=params[1], bold=true, flip=flip);
    }

    // Marked lines
    for (params=[[2, "1"], [4, "2"], [8, "5"], [13, "10"],
                 [18, "15"], [23, "20"], [28, "25"],
                 [36, "45"]])
    {
        translate([params[0] * STOP_SIZE, 0])
        stopMark(length=8, text=flip ? "" : params[1], flip=flip);
    }
    
    // Remaining lines
    for (offset=[1, 3, 5, 7, 9, 10, 11, 12, 14, 15, 16, 17,
                 19, 20, 21, 22, 24, 25, 26, 27, 29, 30, 31,
                 32, 34, 35, 37, 38])
    {
        length = offset < 6 ? 3 : (offset > 33 ? 6 : 4.5);

        translate([offset * STOP_SIZE, 0])
        stopMark(length=length, flip=flip);
    }
}

module stopMark(length=1, text="", bold=false, flip=false) {
    width = bold ? 0.6 : 0.4;
    lineOffset = flip ? -length : 0;
    textOffset = flip ? -10 : 10;
    textValign = flip ? "top" : "bottom";

    translate([-width / 2, lineOffset])
    square(size=[width, length]);

    if (text != "") {
        translate([0, textOffset])
        text(
            text=text,
            size=2.5,
            font=str("Helvetica Neue", bold ? ":style=Bold" : ""),
            halign="center",
            valign=textValign
        );
    }
}

module offMarks() {
    xSize = (OFF_SIZE - 0.5) / 4;
    ySize = 7 / 4;

    // Dots
    for (xOffset=[2:4]) {
        for (yOffset=[0:4]) {
            translate([xOffset * xSize, yOffset * ySize])
            circle(d=xOffset / 4, $fn=50);
        }
    }

    // "OFF" label
    translate([0, ySize * 2])
    rotate(90)
    text(
        text="OFF",
        size=1.75,
        font="Helvetica Neue:style=Condensed Bold",
        halign="center",
        valign="top"
    );
}
