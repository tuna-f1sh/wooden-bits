// Wooden Bits by John Whittington - www.jbrengineering.co.uk
// http://engineer.john-whittington.co.uk/2014/12/wooden-bits-binary-clock/
//
// This work is licensed under the Creative Commons Attribution-NonCommercial 4.0 International License. To view a copy of this license, visit http://creativecommons.org/licenses/by-nc/4.0/// 
// Please include this header with any modification.

// Laser cutter beam kerf
LaserBeamDiameter = 0.23;

// Material characteristic
materialThickness = 3.20;    // Material thickness
// TODO Plastic thickness

// Objects dimensions and other settings
depth = 50; // depth of matrix
hLights = 4; // number of horizontal lights (rows)
vLights = 4; // number of vertical lights (columns)
lightHeight = 50; // height of each light box
lightWidth = 50; // width of each light box
screw_r = 2;
screw_cap = 3.6;
power_r = 4;
button_r = 2;

// --- kerf bend ----
slotLength = 15;
slotWidth = 0.5;
slotDistance = 2; // distance between
slots = depth/slotLength;

// dividers slot
slotDividerLength = 10;
// diffuser slot
dentLength = 10;

// --- working ----
// get the parameter of the curve (half)
parameterCurve = PI*(lightHeight/2)+materialThickness*2;
// length of section top/bottom
lengthSection = ((hLights+1)*materialThickness)+(hLights*lightWidth);
// length of section containing lights
sepLight = lengthSection/hLights;

// --- EXPORT SETTIGS --- 
split = 1; // divide complete strip by this number
spacing = 2; // spacing between cut outs
middle = 0; // whether to make cable holes on top and bottom for middle sections
$fn=100;

// overall length of sheet
/* lengthSheet = (vLights*parameterCurve)+((vLights+1)*(lengthSection+materialThickness))+LaserBeamDiameter; // uncomment for one-piece*/
/* single = 0;*/

// !! comment out for one piece
lengthSheet = parameterCurve+lengthSection*2+LaserBeamDiameter+materialThickness*2; // comment out for one piece
single = 1;

echo("Area Wood:");
echo(( (spacing+lengthSheet)*4 )*((spacing+materialThickness+lightHeight)*4));
echo("Acrylic Area:");
echo((4*( lightHeight+spacing ))*(4*(lightWidth+spacing)));

// comment uncomment for different dxf exports to cut wood sections of acrylic diffusers
/* fullPrint();*/
/* acrylic();*/
/* wood();*/
fullWood();
/* screwPlate();*/
/* lightDivider(2);*/

// --- END EXPORT ---

module body(middle)
{
difference() {
  // make the base sheet
  square([lengthSheet/split, depth]);
  // we're splitting the sheet
  translate([materialThickness/2,0]) { // end section adds material
    if (single == 0) {
        for (z = [1 : hLights/split]) {
          // move along the y (depth) creating columns of slots (depth * 2 to make sure we make it over)
          for (x = [0 : slotLength+slotDistance : depth*2]) {
            slotColumn((lengthSection+materialThickness)*z+(parameterCurve*(z-1)),x);
            slotColumn((lengthSection+materialThickness)*z+slotDistance+(parameterCurve*(z-1)),x+slotLength/2);
          };
        };
    } else { // one block (top bottom and curve)
        // move along the y (depth) creating columns of slots (depth * 2 to make sure we make it over)
        for (x = [0 : slotLength+slotDistance : depth*2]) {
          slotColumn(lengthSection*1+materialThickness,x);
          slotColumn(lengthSection*1+slotDistance+materialThickness,x+slotLength/2);
        };
    };
    if (middle == 1) {
      for (x = [0:3]) { // cable holes at both ends
        translate([sepLight/2+((lengthSection+parameterCurve+materialThickness)*x),depth-10]) circle(2,center=true);
        translate([lengthSection-sepLight/2+((lengthSection+parameterCurve+materialThickness)*x),depth-10]) circle(2,center=true);
        // cut ends off middle sections
        /* translate([-materialThickness/2,0]) square([(lengthSection/2+materialThickness/2), depth]);*/
        /* translate([-materialThickness/2+lengthSection*1.5+parameterCurve+materialThickness*1.5,0]) square([(lengthSection/2+materialThickness/2+LaserBeamDiameter), depth]);*/
      }
    } else if (middle == 2) { // cable holes near curve only
        translate([sepLight/2,depth-10]) circle(2,center=true);
    } else { // cable holes near curve only
        translate([lengthSection-sepLight/2,depth-10]) circle(2,center=true);
    }
  }
  // add a slot at the end for end blanking
  translate([materialThickness/2-LaserBeamDiameter,0])slotDivider();
  // add slots along the number of lights
  translate([materialThickness/2,0]) {
    for (z = [0:vLights/split]) {
      for (x = [0:hLights]) {
        translate([(sepLight*x)+((lengthSection+parameterCurve+materialThickness)*z),0]) slotDivider();
      };
    };
    // alternate dents
    for (z = [0:2:vLights/split]) {
      translate([(lengthSection+parameterCurve+materialThickness)*z,0]) {
        for (x = [0:hLights-1]) {
          translate([sepLight*x,0]) dentB();
          translate([sepLight*x,depth-materialThickness]) dentB();
        };
      };
    };
    for (z = [1:2:vLights/split]) {
      translate([(lengthSection+parameterCurve+materialThickness)*z,0]) {
        for (x = [0:hLights-1]) {
          translate([sepLight*x,depth-materialThickness]) dentB();
          translate([sepLight*x,0]) dentB();
        };
      };
    };
  };
};
};

module fullPrint() {
body(middle);
translate([materialThickness,0]) {
  for (x = [0:hLights]) {
    // we don't want a cable run at the end piece
    if (x < hLights - 2) {
      translate([x*(lightHeight+materialThickness*2+spacing*2),depth+spacing]) lightDivider(1);
    } else {
      translate([x*(lightHeight+materialThickness*2+spacing*2),depth+spacing]) lightDivider(0);
    }
  };
};
/* translate([materialThickness+lengthSection+parameterCurve,-220]) {*/
  translate([0,depth*2+lightHeight+materialThickness*2+LaserBeamDiameter+spacing*3]) backingPlate();
  for (x = [0:hLights-1]) {
    translate([(sepLight)*x,depth*2+spacing*2]) lightDiffuser();
  };
/* };*/
};

module acrylic() {
  hAcrylic = 297;
  wAcrylic = 210;
  for (y = [0:lightHeight+LaserBeamDiameter+materialThickness*2+spacing:wAcrylic-lightHeight]) {
    for (x = [0:lightWidth+LaserBeamDiameter+spacing:hAcrylic-lightWidth]) {
      translate([x,y]) lightDiffuser();
    }
  }
}

module wood(mid) {
body(mid);
translate([materialThickness,0]) {
  for (x = [0:hLights]) {
    // we don't want a cable run at the end piece
    if (x < hLights - 1) {
      translate([x*(lightHeight+materialThickness*2+spacing*2),depth+spacing]) lightDivider(1);
    } else {
      if ((mid == 0) && (x == 4)) {
        translate([x*(lightHeight+materialThickness*2+spacing*2),depth+spacing]) lightDivider(2);
      } else {
        translate([x*(lightHeight+materialThickness*2+spacing*2),depth+spacing]) lightDivider(0);
      }
    }
  };
};
/* translate([materialThickness+lengthSection+parameterCurve,-220]) {*/
  if (mid == 0) {
    translate([(hLights+1)*(lightHeight+materialThickness*2+spacing*2),depth*1+spacing*1+materialThickness]) backingPlate(1);
  } else {
  translate([(hLights+1)*(lightHeight+materialThickness*2+spacing*2),depth*1+spacing*1+materialThickness]) backingPlate(0);
  }
/* };*/
}

module screwPlate() {
  backingPlate(1);
}

module fullWood() {
  for (x = [0:3]) {
    if (x <= 1) {
      translate([0,(depth*2+spacing*3+materialThickness*2)*x]) wood(1);
    } else if (x == 2) {
      translate([0,(depth*2+spacing*3+materialThickness*2)*x]) wood(2);
    } else {
      translate([0,(depth*2+spacing*3+materialThickness*2)*x]) wood(0);
    }
  }
}

// makes column of slots (x direction)
module slotColumn(xpos,ypos)
{
  for (i = [0 : slotDistance*2 : parameterCurve-slotDistance]) {
    translate([xpos+i,ypos]) {
      square([slotWidth-LaserBeamDiameter,slotLength-LaserBeamDiameter],center=true);
    }
  }
}

// makes a pair of slots for light dividers
module slotDivider()
{
  translate([0,depth/2-depth/4]) square([materialThickness-LaserBeamDiameter,slotDividerLength-LaserBeamDiameter],center=true);
  translate([0,depth/2+depth/4]) square([materialThickness-LaserBeamDiameter,slotDividerLength-LaserBeamDiameter],center=true);
}

module dentA()
{
  translate([sepLight/2,materialThickness/2]) square([dentLength-LaserBeamDiameter,materialThickness],center=true);
}

module dentB()
{
  translate([sepLight/4,materialThickness/2]) {
    square([dentLength-LaserBeamDiameter,materialThickness],center=true);
    translate([sepLight/2,0]) square([dentLength-LaserBeamDiameter,materialThickness],center=true);
  }
}

module toothB()
  translate([sepLight/4,-materialThickness/2]) {
    square([dentLength+LaserBeamDiameter,materialThickness],center=true);
    translate([sepLight/2,0]) square([dentLength+LaserBeamDiameter,materialThickness],center=true);
  }

module lightDivider(cable_run)
{
  hdiv = lightHeight+LaserBeamDiameter;
  wdiv = depth+LaserBeamDiameter;
  union() {
    difference() {
      square([hdiv,wdiv-materialThickness]); // take material thickness off back for single backing piece
      if (cable_run == 1) { // mid-pieces have run for able
        translate([hdiv/2,wdiv-materialThickness]) circle(4); // cut a half circle for cable run
      } else if (cable_run == 2) { // one end piece has power connector
        translate([10+power_r, wdiv-10-power_r]) circle(r=power_r,center=true);
        translate([hdiv-15, wdiv-10-power_r]) circle(r=button_r,center=true);
      }
    }
    translate([-materialThickness/2,wdiv/2+wdiv/4]) square([materialThickness+LaserBeamDiameter,slotDividerLength+LaserBeamDiameter],center=true);
    translate([-materialThickness/2,wdiv/2-wdiv/4]) square([materialThickness+LaserBeamDiameter,slotDividerLength+LaserBeamDiameter],center=true);
    translate([hdiv+materialThickness/2,wdiv/2+wdiv/4]) square([materialThickness+LaserBeamDiameter,slotDividerLength+LaserBeamDiameter],center=true);
    translate([hdiv+materialThickness/2,wdiv/2-wdiv/4]) square([materialThickness+LaserBeamDiameter,slotDividerLength+LaserBeamDiameter],center=true);
  }
}

module lightDiffuser()
{
  hdiff = lightHeight+LaserBeamDiameter;
  wdiff = lightWidth+LaserBeamDiameter; // take thickness off for divider
  union() {
    square([wdiff,hdiff]);
    translate([sepLight/4-materialThickness/2,-materialThickness/2]) {
      square([dentLength+LaserBeamDiameter,materialThickness],center=true);
        translate([wdiff/2+materialThickness/2,0]) square([dentLength+LaserBeamDiameter,materialThickness],center=true);
    };
    translate([sepLight/4-materialThickness/2,hdiff+materialThickness/2]) {
      square([dentLength+LaserBeamDiameter,materialThickness],center=true);
        translate([wdiff/2+materialThickness/2,0]) square([dentLength+LaserBeamDiameter,materialThickness],center=true);
    };
  };
}

module backingPlate(wall)
{
  /* length = hLights*lightWidth+LaserBeamDiameter;*/
  length = lengthSection+materialThickness+LaserBeamDiameter;
  height = lightHeight+LaserBeamDiameter;
  difference() {
    union() {
      square([length,height]);
      translate([materialThickness/2,0]) {
        for (x = [0 : sepLight : length-lightWidth]) {
          translate([x,0]) toothB();
        };
        for (x = [0 : sepLight : length-lightWidth]) {
          translate([x,height+materialThickness]) toothB();
        };
      };
    };
    if (wall) {
      translate([sepLight/2,height/5]) {
        screwHole();
      }
      translate([sepLight*3+sepLight/2,height/5]) {
        screwHole();
      }
    }
  }
}

module screwHole() {
  circle(screw_r-LaserBeamDiameter,center=true);
  translate([0,screw_r-LaserBeamDiameter]) square([screw_r*2-LaserBeamDiameter,screw_r*2-LaserBeamDiameter],center=true);
  translate([0,screw_cap+screw_r-LaserBeamDiameter]) circle(screw_cap-LaserBeamDiameter,center=true);
}
