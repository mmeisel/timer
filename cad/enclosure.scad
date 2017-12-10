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
BATTERY_X = 110;
BATTERY_Y = 33;
BATTERY_Z = 20;
BATTERY_DOOR_MARGIN = 2;
BATTERY_DOOR_TAB_X = 8;
BATTERY_DOOR_TAB_Y = 12;


union() {
    backPanel();
    bottomPanel();
    batteryHolder();
}

translate([WIDTH + 20, 0])
batteryDoor();

module bottomPanel() {
    union() {
        difference() {
            edgePanel(WIDTH, DEPTH, THICKNESS);

            translate([THICKNESS + 2, DEPTH - THICKNESS])
            mirror([0, 1])
            {
                batteryDoorCutout();
            }
        }

        // PCB risers
        translate([WIDTH - PCB_SPACING - THICKNESS - 8,
                   DEPTH - PCB_SPACING - THICKNESS - 12,
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
        translate([WIDTH - PCB_SPACING / 2 - THICKNESS - 8,
                   THICKNESS,
                   HEIGHT / 2])
        {
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

module batteryHolder() {
    // Mounting bracket for battery holder, open on the PCB side
    // to allow the wires to go through
    translate([THICKNESS,
               DEPTH - BATTERY_Y - 3 * THICKNESS / 2,
               THICKNESS])
    difference()
    {
        // Add THICKNESS / 2 for interior walls
        cube([BATTERY_X,
              BATTERY_Y + THICKNESS / 2,
              BATTERY_Z + 3 * THICKNESS / 2]);
        
        translate([0, THICKNESS / 2])
        cube([BATTERY_X, BATTERY_Y, BATTERY_Z + THICKNESS]);

        // Screw holes are centered, 56mm apart
        for (x=[BATTERY_X / 2 - 28, BATTERY_X / 2 + 28]) {
            translate([x, BATTERY_Y / 2, BATTERY_Z])
            metric_thread(3, 0.5, THICKNESS * 3);
        }
    }
}

module batteryDoor(cutout=false) {
    margin = cutout ? 2 : 1.9;
    tabX = cutout ? 3 : 2.8;
    tabY = cutout ? 6 : 5.8;
    tabZ = cutout ? (THICKNESS / 2) : 1;

    union() {
        // Door itself
        cube([BATTERY_X + margin * 2,
              BATTERY_Y + margin * 2,
              THICKNESS / 2]);
        
        // Mating tabs
        translate([-2, BATTERY_Y / 4 - tabY / 2, THICKNESS / 2]) {
            cube([tabX, tabY, tabZ]);

            translate([0, BATTERY_Y / 2])
            cube([tabX, tabY, tabZ]);
        }

        // Tab for screw
        translate([BATTERY_X + margin, BATTERY_Y / 2 + margin])
        difference() {
            translate([0, -BATTERY_DOOR_TAB_Y / 2])
            cube([margin + BATTERY_DOOR_TAB_X,
                  BATTERY_DOOR_TAB_Y,
                  THICKNESS / 2]);

            // TODO: check these measurements
            // Screw hole
            translate([margin + BATTERY_DOOR_TAB_X / 2, 0, -THICKNESS])
            cylinder(d=3, h=THICKNESS * 3, $fn=DETAIL);

            // Countersink
            translate([margin + 4, 0, -THICKNESS / 4])
            cylinder(h=THICKNESS / 2, d=4.5, $fn=DETAIL);
        }
    }
}

module batteryDoorCutout() {
    union() {
        // Door itself
        cube([BATTERY_X + BATTERY_DOOR_MARGIN * 2,
              BATTERY_Y + BATTERY_DOOR_MARGIN * 2,
              THICKNESS / 2]);

        // Opening and rails for the door to rest on
        translate([BATTERY_DOOR_MARGIN,
                   BATTERY_DOOR_MARGIN,
                   THICKNESS / 2])
        cube([BATTERY_X, BATTERY_Y, THICKNESS / 2]);

        // Screw receptor
        translate([BATTERY_X + BATTERY_DOOR_MARGIN, BATTERY_Y / 2 + BATTERY_DOOR_MARGIN])
        difference() {
            translate([0, -4])
            cube([BATTERY_DOOR_MARGIN + 6, 8, THICKNESS / 2]);

            // Screw hole
            translate([3, 0, -THICKNESS])
            metric_thread(3, 0.5, THICKNESS * 3);
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