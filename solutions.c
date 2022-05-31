#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#define pi 3.14159265358979323846

typedef struct vehiclepos_struct
{
    int PositionID_int;
    char VehicleRegistration_char[100];
    float Latitude_float;
    float Longitude_float;
    uint64_t UTC_uint64;
} data;

typedef struct closetVehicle_struct
{
    int PositionID_int;
    char VehicleRegistration_char[100];
    float Latitude_float;
    float Longitude_float;
    uint64_t UTC_uint64;
    double distance;
    float difference;
} closeVehicledata;

double deg2rad(double deg);
double rad2deg(double rad);
double distance(double lat1, double lon1, double lat2, double lon2, char unit);
int main()
{
    // to store the execution time of code
    double time_spent = 0.0;

    //Open file
    closeVehicledata vehicle_data;
    FILE *fp1;
    fp1 = fopen("positions.dat","rb");
    if(fp1 == NULL)
    {
        printf("File failed to open!");
        return -1;
    }

    //Seek end of file to get file size
    fseek(fp1,0L,SEEK_END);
    long filesize = ftell(fp1);

    //Assign enough memory to the buffer
    data* vehicleposBuffer = malloc((filesize+1)*sizeof(*vehicleposBuffer));
    data* filtered = malloc((filesize+1)*sizeof(*vehicleposBuffer));

    //Seek begining of file
    fseek(fp1,0,SEEK_SET);

     //Read data from stream/File
     //Scan data into structure
    int amount = fread(vehicleposBuffer,sizeof(*vehicleposBuffer),filesize,fp1);
    if (ferror( fp1 ) != 0 )
    {
        fputs("Error reading file", stderr);
    }


    int cnt = 0;
    int cnt_val = 0;
    int cnt_appear = 0;
    float lat1 = 0.0;
    float lat_v[10] = {34.544909,32.345544,33.234235,35.195739,31.895839
                       ,32.895839,34.115839,32.335839,33.535339,32.234235
                      };
    float lon_v[10] = {-102.100843,-99.123124,-100.214124,-95.348899,-97.789573
                       ,-101.789573,-100.225732,-99.992232,-94.792232,-100.222222
                      };
    float lon2 =0.0 ;
    float furthest_point[10] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0};
    float lowestfurthestPoint = 0.0;
//    int lowestfuthestIDs[10] = {0,0,0,0,0,0,0};
    //int lowestfuthestID = 0;
    int mostCenterPos = 0;
    int center_loc = 0;
    int positions = 0;
    float dist1  =0.0;
    float dist  =0.0;
    double value = 0.0;

    //Create search radius
    for(center_loc = 0; center_loc<10; center_loc++)
    {
        //Clear array
        memset(furthest_point,0,10*sizeof(furthest_point[0]));

        //Loop through all 10 vehicle position and collect the futherst point from each
        for(positions = 0 ; positions <10; positions++)
        {
            //Avoid measuring distance between the same positions
            if(positions != center_loc)
            {
                lat1 = fabs(lat_v[positions] - lat_v[center_loc]);
                lon2 = fabs(lon_v[positions] + fabs(lon_v[center_loc]));

                //Store furthest point from center point
                if((lat1 + lon2) > furthest_point[center_loc])
                {
                    furthest_point[center_loc] = (lat1 + lon2);
//                    lowestfuthestIDs[center_loc] = positions;
                }
            }
        }
        //Find the center location with the lowest furtherst point
        if(furthest_point[center_loc] < lowestfurthestPoint && lowestfurthestPoint != 0)
        {
            lowestfurthestPoint = furthest_point[center_loc];
            mostCenterPos = center_loc;
//            lowestfuthestID = lowestfuthestIDs[center_loc];
        }
        else if(lowestfurthestPoint == 0)
        {
            lowestfurthestPoint = furthest_point[center_loc];
            mostCenterPos = center_loc;
        //    lowestfuthestID = lowestfuthestIDs[center_loc];
        }
    }

    //Start measuring clock
    clock_t begin = clock();

    //Loop through all 10 vehicle positions
    for(int cnt1 = 0; cnt1<10; cnt1++)
    {
        //Loop through all the points that fall in the search radius
        for(cnt = 0; cnt<amount; cnt++)
        {
            //Calculate the distance between the Vehicles
            dist1 = fabs(vehicleposBuffer[cnt].Latitude_float - lat_v[cnt1]) + fabs(vehicleposBuffer[cnt].Longitude_float + fabs(lon_v[cnt1]));

            //Capture the nearest vehicle
            if(cnt != 0 && dist1 < vehicle_data.difference)
            {
                vehicle_data.difference = dist1;
                cnt_val = cnt;
            }

            if(cnt == 0)
            {
                vehicle_data.difference = dist1;
            }

            if(cnt1 == 0)
            {
                //Calculate the distance between the center position and the vehicle
                dist = fabs(vehicleposBuffer[cnt].Latitude_float - lat_v[mostCenterPos]) + fabs(vehicleposBuffer[cnt].Longitude_float + fabs(lon_v[mostCenterPos]));

                //Check if point fall in the search radius
                if(dist < lowestfurthestPoint)
                {
                    //Capture points that fall in the radius
                    filtered[cnt_appear] = vehicleposBuffer[cnt];

                    //Count number of points that fall in the radius
                    cnt_appear++;
                }
            }

        }
        //Calculate the actual distance between the vehicles
        value = distance(lat_v[cnt1],lon_v[cnt1],vehicleposBuffer[cnt_val].Latitude_float,vehicleposBuffer[cnt_val].Longitude_float,'K');
        vehicle_data.PositionID_int= vehicleposBuffer[cnt_val].PositionID_int;
        //vehicle_data.VehicleRegistration_char[0] = 's';
        vehicle_data.Latitude_float = vehicleposBuffer[cnt_val].Latitude_float;
        vehicle_data.Longitude_float = vehicleposBuffer[cnt_val].Longitude_float;
        vehicle_data.distance = value;

        printf("Closest Position Id: %d \tDistance %fkm\n",vehicle_data.PositionID_int,vehicle_data.distance);
        if(cnt1 == 0)
        {
            //Assign the vehicle points that fall inside the search radius
            //memcpy(vehicleposBuffer, filtered,cnt_appear*sizeof(*filtered));
            vehicleposBuffer = filtered;
            amount = cnt_appear;
        }

    }
    clock_t end = clock();
    time_spent = (float)(end - begin) / CLOCKS_PER_SEC;
    printf("The elapsed time is %f seconds", time_spent);

    //Clear buffers from memory
    free(filtered);
    free(vehicleposBuffer);
    //Close file
    fclose(fp1);
    return 0;
}
double distance(double lat1, double lon1, double lat2, double lon2, char unit)
{
    double theta, dist;
    if ((lat1 == lat2) && (lon1 == lon2))
    {
        return 0;
    }
    else
    {
        theta = lon1 - lon2;
        dist = sin(deg2rad(lat1)) * sin(deg2rad(lat2)) + cos(deg2rad(lat1)) * cos(deg2rad(lat2)) * cos(deg2rad(theta));
        dist = acos(dist);
        dist = rad2deg(dist);
        dist = dist * 60 * 1.1515;
        switch(unit)
        {
        case 'M':
            break;
        case 'K':
            dist = dist * 1.609344;
            break;
        case 'N':
            dist = dist * 0.8684;
            break;
        }
        return (dist);
    }
}
double deg2rad(double deg)
{
    return (deg * pi / 180);
}
double rad2deg(double rad)
{
    return (rad * 180 / pi);
}
