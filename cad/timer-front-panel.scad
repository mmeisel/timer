use <threads.scad>

// Constants
OFF_SIZE = 6400 / 1024;
STOP_SIZE = 2400 / 1024;
MARK_DEPTH = 0.2;

union() {
    panel();
    detentes();
}

module panel() {
    difference() {
        // The panel itself
        color("white")
        cube(size=[170, 52, 1.5], center=true);

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
        color("black")
        translate([-50, 1.5, 0.75])
        offMarks();
    }
}

module detentes() {
    size = 2;
    // The gap between the panel and the knob is between
    // 2.2 and 3.2 mm according to the spec sheet.
    // So nothing above ~2 for the height.
    height = 1.75;
    gap = 0.25;
    xOffset = -50 + OFF_SIZE + STOP_SIZE / 2;

    for (stop=[-1:39]) {
        color("white")
        translate([xOffset + stop * STOP_SIZE, -2.5 - size, 0.75])
        {
            points = [
                [gap, 0, 0],
                [STOP_SIZE - gap, 0, 0],
                [STOP_SIZE - gap, size, 0],
                [gap, size, 0],
                [stop == -1 ? STOP_SIZE / 2 : gap, 0, height],
                [stop == -1 ? STOP_SIZE / 2 : gap, size, height]
            ];

            faces = [
                [0, 1, 2, 3],   // bottom
                [4, 1, 0],      // front
                [4, 5, 2, 1],   // top
                [5, 2, 3],      // back
                [4, 5, 3, 0]    // left
            ];
            
            polyhedron(points, faces);
        }
    }
}

module stopSequence(flip=false) {
    // Bold numbers (zero and scale changes)
    for (params=[[0, "0"], [6, "3"], [33, "30"], [39, "60"]]) {
        translate([params[0] * STOP_SIZE, 0])
        stopMark(length=8, text=params[1], bold=true, flip=flip);
    }

    // Marked lines
    for (params=[[2, "1"], [4, "2"], [8, "5"], [13, "10"],
                 [18, "15"], [23, "20"], [28, "25"],
                 [36, "45"]])
    {
        translate([params[0] * STOP_SIZE, 0])
        stopMark(length=7, text=flip ? "" : params[1], flip=flip);
    }
    
    // Remaining lines
    for (offset=[1, 3, 5, 7, 9, 10, 11, 12, 14, 15, 16, 17,
                 19, 20, 21, 22, 24, 25, 26, 27, 29, 30, 31,
                 32, 34, 35, 37, 38])
    {
        length = offset < 6 ? 2 : (offset > 33 ? 5 : 3.5);

        translate([offset * STOP_SIZE, 0])
        stopMark(length=length, flip=flip);
    }
}

module stopMark(length=1, text="", bold=false, flip=false) {
    width = bold ? 0.4 : 0.2;
    lineOffset = flip ? -8 : (8 - length);
    textOffset = flip ? -9 : 9;
    textValign = flip ? "top" : "bottom";

    translate([-width / 2, lineOffset, -MARK_DEPTH])
    cube(size=[width, length, MARK_DEPTH]);

    if (text != "") {
        translate([0, textOffset, -MARK_DEPTH])
        linear_extrude(height=MARK_DEPTH)
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
    ySize = 6 / 4;

    for (yTranslation=[1.5, -10.5]) {
        translate([0, yTranslation, -MARK_DEPTH])
        linear_extrude(height=MARK_DEPTH)
        {
            // Dots
            for (xOffset=[2:4]) {
                for (yOffset=[0:4]) {
                    // Leave out the dots covered by the detente
                    // on the bottom
                    if (yTranslation != -10.5 ||
                        xOffset != 4 || yOffset < 3)
                    {
                        translate([xOffset * xSize, yOffset * ySize])
                        circle(d=xOffset / 4, $fn=50);
                    }
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
    }
}
