//COMMANDS:
//http://planner.ardupilot.com/wiki/common-mavlink-mission-command-messages-mav_cmd/#commands_supported_by_copter

//SOURCE CODE FOR ARDUPILOT/APM:
//https://github.com/diydrones

//IDEAL USAGE:
//Step 1) "route1" text is read which will call
//Step 2) route_one.c which will get to the MAV> prompt and
//Step 3) issue command "mode auto" from which:
//Step 4) AUTO> <start c code to issue each of the commands below in route_one>

//refresher for c/unix: system("echo 'words' > file"); //c code to do unix stuff

//fly in a square at altitude 3 feet in the north side of the east parking lot of SPN
//coordinates can be confirmed on google maps
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
