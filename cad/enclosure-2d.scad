// Constants
THICKNESS = 2.7;
DETAIL = 50;

// This should be half the laser's actual kerf. That is, the amount
// of material lost on *one side* of a cut.
KERF = 0.1;

PANEL_WIDTH = 170;
PANEL_HEIGHT = 52;
PANEL_DEPTH = 1.5;

WIDTH = PANEL_WIDTH + 2 * THICKNESS + 2 * KERF;
HEIGHT = PANEL_HEIGHT + 2 * THICKNESS + 2 * KERF;
DEPTH = 80 + 2 * KERF;

SPEAKER_DIAMETER = 36;
PCB_SPACING = 39.37;    // Distance between mounting holes on PCB
DOOR_TAB_WIDTH = 16;
HINGE_TAB_WIDTH = 4;

BATTERY_X = 110;
BATTERY_Y = 33;
BATTERY_Z = 20;

backPanel();

translate([0, HEIGHT + 5])
topPanel();

module backPanel() {
    difference() {
        // Panel
        square(size=[WIDTH, HEIGHT]);

        // Tab
        translate([0, HEIGHT - THICKNESS + KERF])
        square([WIDTH / 2 - DOOR_TAB_WIDTH / 2 - KERF,
                THICKNESS - KERF]);

        translate([WIDTH / 2 + DOOR_TAB_WIDTH / 2 + KERF,
                   HEIGHT - THICKNESS + KERF])
        square([WIDTH / 2 - DOOR_TAB_WIDTH / 2 - KERF,
                THICKNESS - KERF]);

        // Hinge tab, left
        translate([0, HINGE_TAB_WIDTH + KERF])
        square([THICKNESS - KERF, HEIGHT - HINGE_TAB_WIDTH - KERF]);

        // Hinge tab, right
        translate([WIDTH - THICKNESS + KERF, HINGE_TAB_WIDTH - KERF])
        square([THICKNESS - KERF, HEIGHT - HINGE_TAB_WIDTH - KERF]);

    }
}

module topPanel() {
    difference() {
        // Panel
        square(size=[WIDTH, DEPTH]);

        // Finger joints

        // Left
        rotate(90)
        fingerCuts(width=DEPTH, startUp=true);
        
        // Right
        translate([WIDTH, DEPTH])
        rotate(-90)
        fingerCuts(width=DEPTH, startUp=true);

        // Front
        translate([WIDTH, 0])
        rotate(180)
        fingerCuts(width=WIDTH, startUp=false);

        // Cutout for battery door tab
        translate([WIDTH / 2 - DOOR_TAB_WIDTH / 2 + KERF,
                   DEPTH - THICKNESS + KERF])
        square([DOOR_TAB_WIDTH - KERF, THICKNESS - KERF]);

        // Speaker holes
        translate([WIDTH / 2, DEPTH / 2]) {
            intersection() {
                union() {
                    for (yOffset=[-15 : 3 : 15]) {
                        translate([-16, yOffset])
                        square([32 - 2 * KERF, 0.5 - 2 * KERF]);
                    }
                }
 
                circle(d=32 - 2 * KERF, $fn=DETAIL);
            }
        }
    }
}

module fingerCuts(width=10,
                  fingerWidth=THICKNESS,
                  depth=THICKNESS,
                  startUp=true)
{
    fingers = floor(width / (2 * fingerWidth) - 1);
    totalFingerWidth = fingerWidth * (2 * fingers - 1);

    // When startUp is false, cut all the way to the edge on both ends.
    if (!startUp) {
        translate([0, -depth + KERF])
        square([(width - totalFingerWidth) / 2 - KERF, depth - KERF]);

        translate([width - (width - totalFingerWidth) / 2 + KERF,
                   -depth + KERF])
        square([(width - totalFingerWidth) / 2 - KERF, depth - KERF]);
    }

    translate([(width - totalFingerWidth) / 2, 0]) {
        for (i=[(startUp ? 0 : 1) : 2 : 2 * fingers - 1]) {
            translate([i * fingerWidth + KERF, -depth + KERF])
            square([fingerWidth - 2 * KERF, depth - KERF]);
        }
    }
}

/*
module roundedSquare(size=[10, 10], r=1, center=false) {
    hull() {
        for (x=[r, size[0] - r]) {
            for (y=[r, size[1] - r]) {
                translate([x, y]) circle(r=r, $fn=DETAIL);
            }
        }
    }
}
*/
