// Constants
THICKNESS = 2.7;
DETAIL = 50;

// This should be half the laser's actual kerf. That is, the amount
// of material lost on *one side* of a cut.
KERF = 0.1;

FACE_WIDTH = 170;
FACE_HEIGHT = 52;
FACE_DEPTH = 1.5;

WIDTH = FACE_WIDTH + 2 * THICKNESS;
HEIGHT = FACE_HEIGHT + 2 * THICKNESS;
DEPTH = 80;

WITH_FRONT_PANEL = false;

INTERIOR_PANEL_DEPTH = THICKNESS * 3;
INTERIOR_TAB_WIDTH = THICKNESS * 3;
SPEAKER_DIAMETER = 36;
PCB_SPACING = 39.37;    // Distance between mounting holes on PCB
PCB_SIZE = 46.99;
DOOR_TAB_WIDTH = 16;
MAGNET_THICKNESS = 4;
HINGE_HOLE_D = sqrt(2 * THICKNESS * THICKNESS);
HINGE_HEIGHT = 4 * THICKNESS;
BOTTOM_DEPTH = DEPTH - HINGE_HOLE_D / 2 - 0.5;    // 0.5mm clearance
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

translate([KERF, HEIGHT + DEPTH + 10])
bottomPanel();

translate([KERF, HEIGHT + DEPTH + BOTTOM_DEPTH + 15])
interiorPanel();

translate([KERF, 2 * HEIGHT + DEPTH + BOTTOM_DEPTH + FRONT_HEIGHT + 10])
sidePanel();

translate([KERF + WIDTH,
           2 * HEIGHT + DEPTH + BOTTOM_DEPTH + FRONT_HEIGHT + 10])
mirror()
sidePanel();

if (WITH_FRONT_PANEL) {
    translate([KERF,
               2 * HEIGHT + 2 * DEPTH + BOTTOM_DEPTH + 10])
    frontPanel();
}


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
        if (WITH_FRONT_PANEL) {
            translate([WIDTH, 0])
            rotate(180)
            fingerCuts(width=WIDTH, startUp=false);
        }

        // Cutout for battery door tab
        translate([(WIDTH - DOOR_TAB_WIDTH) / 2,
                   DEPTH - THICKNESS - MAGNET_THICKNESS])
        offset(-KERF)
        square([DOOR_TAB_WIDTH, 2 * THICKNESS + MAGNET_THICKNESS]);

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
            if (WITH_FRONT_PANEL) {
                rotate(90)
                fingerCuts(width=HEIGHT, startUp=false);
            }

            // Cutout for hinge hole
            translate([DEPTH, HINGE_HOLE_D / 2])
            circle(d=HINGE_HOLE_D + KERF, $fn=DETAIL);

            // Cutout for interior panel tab
            translate([INTERIOR_PANEL_DEPTH + KERF,
                       HEIGHT / 2 - INTERIOR_TAB_WIDTH / 2 + KERF])
            square([THICKNESS, INTERIOR_TAB_WIDTH - 2 * KERF]);
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
            translate([-KERF, -KERF])
            square([THICKNESS, 2 * BOTTOM_DEPTH]);

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
            square([THICKNESS, 2 * BOTTOM_DEPTH]);

            rotate(90) union() {
                frontFoot();
                hingeHousing();
            }
        }

        // Bottom (front)
        if (WITH_FRONT_PANEL) {
            translate([WIDTH, 0])
            rotate(180)
            fingerCuts(width=WIDTH, startUp=false);
        }

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

        // Holes for tabs from interior panel
        for (xOffset=[WIDTH / 8, WIDTH / 2, 7 * WIDTH / 8]) {
            translate([xOffset - INTERIOR_TAB_WIDTH / 2 + KERF,
                       INTERIOR_PANEL_DEPTH + KERF])
            square([INTERIOR_TAB_WIDTH - 2 * KERF,
                    THICKNESS - 2 * KERF]);
        }

        // PCB mounting point
        translate([WIDTH - THICKNESS - PCB_SPACING - 8,
                   BOTTOM_DEPTH - PCB_SPACING - 8])
        {
            for (xOffset=[0, PCB_SPACING]) {
                for (yOffset=[0, PCB_SPACING]) {
                    translate([xOffset, yOffset])
                    circle(d=3, $fn=DETAIL);
                }
            }

            % translate([(PCB_SIZE - PCB_SPACING) / -2,
                         (PCB_SIZE - PCB_SPACING) / -2])
            square(PCB_SIZE);
        }

        // Battery holder mounting point
        // Screw holes are centered, 56mm apart
        translate([THICKNESS + (BATTERY_X - 56) / 2 + 4,
                   BOTTOM_DEPTH - BATTERY_Y / 2 - 3])
        circle(d=3, $fn=DETAIL);

        translate([THICKNESS + (BATTERY_X + 56) / 2 + 4,
                   BOTTOM_DEPTH - BATTERY_Y / 2 - 3])
        circle(d=3, $fn=DETAIL);

        % translate([THICKNESS + 4,
                   BOTTOM_DEPTH - BATTERY_Y - 4])
        square([BATTERY_X, BATTERY_Y]);
    }
}

module frontPanel() {
    difference() {
        kerfAdjustedPanel([WIDTH, HEIGHT]);

        // Top
        translate([0, HEIGHT])
        fingerCuts(width=WIDTH, startUp=true);

        // Left
        rotate(90)
        fingerCuts(width=HEIGHT, startUp=true);
        
        // Right
        translate([WIDTH, HEIGHT])
        rotate(-90)
        fingerCuts(width=HEIGHT, startUp=true);

        // Bottom
        translate([WIDTH, 0])
        rotate(180)
        fingerCuts(width=WIDTH, startUp=true);

        // Front panel hole
        translate([WIDTH / 2, HEIGHT / 2])
        square([FACE_WIDTH - 12, FACE_HEIGHT - 12], center=true);
    }
}

module interiorPanel() {
    difference() {
        kerfAdjustedPanel([WIDTH, HEIGHT - THICKNESS]);

        // Side tabs
        for (xOffset=[-KERF, WIDTH - THICKNESS + KERF]) {
            translate([xOffset, -KERF])
            square([THICKNESS,
                    (HEIGHT - INTERIOR_TAB_WIDTH - KERF) / 2]);

            translate([xOffset, (HEIGHT + INTERIOR_TAB_WIDTH) / 2 - KERF])
            square([THICKNESS,
                    (HEIGHT - INTERIOR_TAB_WIDTH - KERF) / 2]);
        }

        // Bottom tabs, outer cutouts
        for (xOffset=[-KERF * 2,
                      7 * WIDTH / 8 + INTERIOR_TAB_WIDTH / 2 + KERF])
        {
            translate([xOffset, -KERF])
            square([WIDTH / 8 - INTERIOR_TAB_WIDTH / 2, THICKNESS]);
        }

        // Bottom tabs, inner cutouts
        for (xOffset=[WIDTH / 8 + INTERIOR_TAB_WIDTH / 2 - KERF,
                      WIDTH / 2 + INTERIOR_TAB_WIDTH / 2 - KERF])
        {
            translate([xOffset, -KERF])
            square([3 * WIDTH / 8 - INTERIOR_TAB_WIDTH - 2 * KERF,
                    THICKNESS]);
        }

        // Hole for slider
        // Measurements come from data sheet, plus some wiggle room
        translate([WIDTH / 2 - 81, HEIGHT / 2 - 6])
        square([152, 14]);

        // Screw holes for face plate
        for (xOffset=[THICKNESS + 6, WIDTH - THICKNESS - 6]) {
            for (yOffset=[THICKNESS + 6, HEIGHT - THICKNESS - 6]) {
                translate([xOffset, yOffset])
                circle(d=3, $fn=DETAIL);
            }
        }
    }
}

module kerfAdjustedPanel(size=[1, 1]) {
    offset(KERF) square(size);
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
