// Constants
THICKNESS = 2.7;
DETAIL = 50;

// This should be half the laser's actual kerf. That is, the amount
// of material lost on *one side* of a cut.
KERF = 0.1;

PANEL_WIDTH = 170;
PANEL_HEIGHT = 52;
PANEL_DEPTH = 1.5;

WIDTH = PANEL_WIDTH + 2 * THICKNESS;
HEIGHT = PANEL_HEIGHT + 2 * THICKNESS;
DEPTH = 80;

SPEAKER_DIAMETER = 36;
PCB_SPACING = 39.37;    // Distance between mounting holes on PCB
DOOR_TAB_WIDTH = 16;
MAGNET_THICKNESS = 4;
HINGE_HOLE_D = sqrt(2 * THICKNESS * THICKNESS);
HINGE_HEIGHT = 4 * THICKNESS;
BOTTOM_DEPTH = DEPTH - HINGE_HOLE_D / 2;
FRONT_HEIGHT = 20;
TILT_ANGLE = atan2(DEPTH, (FRONT_HEIGHT + HINGE_HOLE_D / 2 -
                           HINGE_HEIGHT / 2));

BATTERY_X = 110;
BATTERY_Y = 33;
BATTERY_Z = 20;

translate([KERF, KERF])
backPanel();

translate([KERF, HEIGHT + 5])
topPanel();

translate([WIDTH + 20, FRONT_HEIGHT])
sidePanel();

translate([WIDTH + DEPTH + 20, HEIGHT + FRONT_HEIGHT + 20])
mirror()
sidePanel();

translate([KERF, HEIGHT + DEPTH + 10])
bottomPanel();

// TODO: add signature or something engraved on the bottom

module backPanel() {
    difference() {
        kerfAdjustedPanel([WIDTH, HEIGHT]);

        // Tab
        translate([0, HEIGHT - THICKNESS + KERF])
        square([WIDTH / 2 - DOOR_TAB_WIDTH / 2, THICKNESS]);

        translate([WIDTH / 2 + DOOR_TAB_WIDTH / 2 + KERF,
                   HEIGHT - THICKNESS + KERF])
        square([WIDTH / 2 - DOOR_TAB_WIDTH / 2, THICKNESS]);

        // Hinge tab, left
        translate([-KERF, THICKNESS + KERF])
        square([THICKNESS, HEIGHT]);

        // Hinge tab, right
        translate([WIDTH - THICKNESS + KERF, THICKNESS + KERF])
        square([THICKNESS, HEIGHT]);

    }
}

module topPanel() {
    difference() {
        // Panel
        kerfAdjustedPanel([WIDTH, DEPTH]);

        // Left
        rotate(90)
        fingerCuts(width=DEPTH, startUp=true);
        
        // Right
        translate([WIDTH, DEPTH])
        rotate(-90)
        fingerCuts(width=DEPTH, startUp=true);

        // Bottom (front)
        translate([WIDTH, 0])
        rotate(180)
        fingerCuts(width=WIDTH, startUp=false);

        // Cutout for battery door tab
        translate([(WIDTH - DOOR_TAB_WIDTH) / 2 + KERF,
                   DEPTH - THICKNESS - MAGNET_THICKNESS + KERF])
        square([DOOR_TAB_WIDTH - 2 * KERF,
                THICKNESS + MAGNET_THICKNESS]);

        // Speaker holes
        // TODO: replace with a nice design instead
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

module sidePanel() {
    //rotate(-90 + TILT_ANGLE)
    union() {
        difference()
        {
            kerfAdjustedPanel([DEPTH, HEIGHT]);

            // Top
            translate([0, HEIGHT])
            fingerCuts(width=DEPTH, startUp=false);

            // Bottom
            translate([BOTTOM_DEPTH, 0])
            rotate(180)
            fingerCuts(width=BOTTOM_DEPTH, startUp=false);

            // Front
            rotate(90)
            fingerCuts(width=HEIGHT, startUp=false);

            // Cutout for hinge hole
            translate([DEPTH, HINGE_HOLE_D / 2])
            circle(d=HINGE_HOLE_D + KERF, $fn=DETAIL);
        }

        hingeHousing();
        frontFoot();
    }
}

module bottomPanel() {
    difference() {
        kerfAdjustedPanel([WIDTH, BOTTOM_DEPTH]);

        // Left
        rotate(90)
        fingerCuts(width=BOTTOM_DEPTH, startUp=true);
        
        // Cutouts for side features (left)
        intersection() {
            square([THICKNESS - KERF, BOTTOM_DEPTH]);

            mirror([-1, 1]) union() {
                frontFoot();
                hingeHousing();
            }
        }

        // Right
        translate([WIDTH, BOTTOM_DEPTH])
        rotate(-90)
        fingerCuts(width=BOTTOM_DEPTH, startUp=true);

        // Cutouts for side features (right)
        translate([WIDTH, 0])
        intersection()
        {
            translate([-THICKNESS + KERF, 0])
            square([THICKNESS, BOTTOM_DEPTH]);

            rotate(90) union() {
                frontFoot();
                hingeHousing();
            }
        }

        // Bottom (front)
        translate([WIDTH, 0])
        rotate(180)
        fingerCuts(width=WIDTH, startUp=false);

        intersection() {
            translate([WIDTH - THICKNESS, 0])
            square([THICKNESS, BOTTOM_DEPTH]);

            union() {
                translate([WIDTH + FRONT_HEIGHT, 0])
                rotate(90)
                frontFoot();

                translate([WIDTH - HINGE_HOLE_D / 2, DEPTH])
                rotate(90)
                hingeHousing();
            }
        }

        // PCB mounting point
        translate([WIDTH - THICKNESS - PCB_SPACING - 20,
                   BOTTOM_DEPTH - THICKNESS - PCB_SPACING - 10])
        {
            for (xOffset=[0, PCB_SPACING]) {
                for (yOffset=[0, PCB_SPACING]) {
                    translate([xOffset, yOffset])
                    circle(d=3, $fn=DETAIL);
                }
            }
        }

        // Battery holder mounting point
    }
}

module kerfAdjustedPanel(size=[1, 1]) {
    translate([-KERF, -KERF])
    square([size[0] + 2 * KERF, size[1] + 2 * KERF]);
}

module hingeHousing() {
    translate([DEPTH, HINGE_HOLE_D / 2])
    difference() {
        union() {
            circle(d=HINGE_HEIGHT + KERF, $fn=DETAIL);

            // Block the entire partially occluded finger
            translate([-15, -HINGE_HOLE_D / 2 - KERF])
            square([12, 2 * THICKNESS]);
        }
        circle(d=HINGE_HOLE_D + KERF, $fn=DETAIL);
    }
}



module frontFoot() {
    rotate(90 - TILT_ANGLE)
    translate([13, -FRONT_HEIGHT])
    offset(KERF)
    polygon([[5, 0],
             [0, FRONT_HEIGHT + THICKNESS],
             [20, FRONT_HEIGHT + THICKNESS],
             [15, 0]]);    
}

module fingerCuts(width=10,
                  fingerWidth=2 * THICKNESS,
                  depth=THICKNESS,
                  startUp=true)
{
    fingers = floor(width / (2 * fingerWidth) - 1);
    totalFingerWidth = fingerWidth * (2 * fingers - 1);

    // When startUp is false, cut all the way to the edge on both ends.
    if (!startUp) {
        translate([-KERF, -depth + KERF])
        square([(width - totalFingerWidth) / 2, depth * 2]);

        translate([width - (width - totalFingerWidth) / 2 + KERF,
                   -depth + KERF])
        square([(width - totalFingerWidth) / 2, depth * 2]);
    }

    translate([(width - totalFingerWidth) / 2, 0]) {
        for (i=[(startUp ? 0 : 1) : 2 : 2 * fingers - 1]) {
            translate([i * fingerWidth + KERF, -depth + KERF])
            square([fingerWidth - 2 * KERF, depth * 2]);
        }
    }
}
