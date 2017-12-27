// Constants
// The first and last 2 (+-1) mm of travel don't affect the output
// according to the datasheet
OFF_SIZE = 96 * 64 / 1024;
STOP_SIZE = 96 * 24 / 1024;

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
    translate([-48 + OFF_SIZE + STOP_SIZE / 2, 0])
    {
        translate([0, 3.5]) stopSequence();
        translate([0, -3.5]) stopSequence(flip=true);
    }

    // OFF markings
    translate([-48, 3.5]) offMarks();
    translate([-48, -3.5]) offMarks(flip=true);
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

module stopMark(length=1, text="", bold=false, small=false, flip=false) {
    width = bold ? 0.6 : 0.4;
    lineOffset = flip ? -length : 0;
    textOffsetAmount = small ? 11.25 : 11;
    textOffset = flip ? -textOffsetAmount : textOffsetAmount;
    textValign = flip ? "top" : "bottom";
    textStyle = small ? "Condensed Bold" : (bold ? "Bold" : "Regular");
    textSize = small ? 2.5 : 3;

    translate([-width / 2, lineOffset])
    square(size=[width, length]);

    if (text != "") {
        translate([0, textOffset])
        text(
            text=text,
            size=textSize,
            font=str("Helvetica Neue:style=", textStyle),
            halign="center",
            valign=textValign
        );
    }
}

module offMarks(flip=false) {
    xSize = (OFF_SIZE + STOP_SIZE / 2) / 4;
    ySize = 7 / (flip ? -4 : 4);

    // Dots
    for (xOffset=[1:3]) {
        for (yOffset=[0.5:4.5]) {
            translate([xOffset * xSize, yOffset * ySize])
            circle(d=0.75, $fn=50);
        }
    }

    // "OFF" label
    stopMark(length=9, text="OFF", small=true, bold=true, flip=flip);
}
