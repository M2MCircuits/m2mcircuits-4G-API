//fly in a square at altitude 3 feet in the north side of the east parking lot of SPN
//coordinates can be confirmed on google maps
//http://planner.ardupilot.com/wiki/common-mavlink-mission-command-messages-mav_cmd/#mav_cmd_nav_takeoff

float route_one[][6] = {
	{MAV_CMD_MISSION_START,  	0, 		0,  	0,  	0,	0, 		  	0}//start in AUTO mode without need to touch the throttle on the controller
	//	CMD					P1		P2		P3		Alt		Lat			Long
	{MAV_CMD_NAV_TAKEOFF,  	0, 		0,  	0,  	3.0,	0, 		  	0},					//  go up three feet
	
	{MAV_CMD_NAV_WAYPOINT, 	0, 		0,		0,  	3.0, 	32.994596, -96.751628},			//  south-west corner
	{MAV_CMD_NAV_WAYPOINT, 	0, 		0,		0,  	3.0, 	32.994596, -96.751528},			//  south-east corner
	{MAV_CMD_NAV_WAYPOINT, 	0, 		0,		0,  	3.0, 	32.994696, -96.751528},			//  north-east corner
	{MAV_CMD_NAV_WAYPOINT, 	0, 		0,		0,  	3.0, 	32.994696, -96.751628},			//  north-west corner

	{MAV_CMD_NAV_LAND, 	 	0,		0,		0,  	0,		0,          0},					//  go back down three feet
};
