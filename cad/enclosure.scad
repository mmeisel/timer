use <threads.scad>

// Constants
PANEL_WIDTH = 170;
PANEL_HEIGHT = 52;
PANEL_DEPTH = 1.5;
DEPTH = 80;
HEIGHT = 60;
THICKNESS = 4;
WIDTH = PANEL_WIDTH + 2 * THICKNESS;
DETAIL = 50;
PCB_SPACING = 39.37;    // Distance between mounting holes on PCB

union() {
    bottomPanel();
    backPanel();
}

module bottomPanel() {
    union() {
        batteryX = 110;
        batteryY = 33;
        batteryZ = 20;

        difference() {
            edgePanel(WIDTH, DEPTH, THICKNESS);

            // Hole for 106mm x 32mm x 18mm battery holder
            translate([THICKNESS,
                       DEPTH - THICKNESS - batteryY,
                       -THICKNESS])
            cube([batteryX, batteryY, THICKNESS * 3]);
        }

        // Mounting bracket for battery holder, open on the
        // PCB side to allow the wires to go through
        translate([THICKNESS,
                   DEPTH - batteryY - 3 * THICKNESS / 2,
                   THICKNESS])
        difference()
        {
            // Add THICKNESS / 2 for interior walls
            cube([batteryX,
                  batteryY + THICKNESS / 2,
                  batteryZ + 3 * THICKNESS / 2]);
            
            translate([0, THICKNESS / 2])
            cube([batteryX, batteryY, batteryZ + THICKNESS]);

            // Screw holes are centered, 56mm apart
            for (x=[batteryX / 2 - 28, batteryX / 2 + 28]) {
                translate([x, batteryY / 2, batteryZ])
                metric_thread(3, 0.5, THICKNESS * 3);
            }
        }

        // PCB risers
        translate([WIDTH - PCB_SPACING - THICKNESS - 8,
                   DEPTH - PCB_SPACING - THICKNESS - 16,
                   THICKNESS])
        {
            pcbRiser();

            translate([0, PCB_SPACING])
            pcbRiser();

            translate([PCB_SPACING, 0])
            pcbRiser();

            translate([PCB_SPACING, PCB_SPACING])
            pcbRiser();
        }
    }
}

module backPanel() {
    translate([0, DEPTH])
    difference() {
        // Panel
        rotate([90, 0])
        edgePanel(WIDTH, HEIGHT, THICKNESS);
        
        // Speaker holes, speaker diameter is 36mm
        translate([WIDTH - PCB_SPACING / 2 - 12, THICKNESS, HEIGHT / 2]) {
            intersection() {
                union() {
                    for (zOffset=[-15 : 3 : 15]) {
                        translate([-16, -THICKNESS * 3, zOffset])
                        cube([32, THICKNESS * 3, 0.5]);
                    }
                }
 
                rotate([90, 0])
                cylinder(d=32, h=THICKNESS * 3, $fn=DETAIL);
            }
        }
    }
}

module edgePanel(width=1, depth=1, thickness=1) {
    intersection() {
        cube([width, depth, thickness]);

        translate([0, 0, thickness])
        hull() {
            for (x=[thickness, width - thickness]) {
                for (y=[thickness, depth - thickness]) {
                    translate([x, y])
                    sphere(r=thickness, $fn = DETAIL);
                }
            }
        }
    }
}

module pcbRiser() {
    difference() {
        cylinder(d1=8, d2=6, h=8, $fn=8);

        translate([0, 0, 2]) metric_thread(3, 0.5, 7);
    }
}