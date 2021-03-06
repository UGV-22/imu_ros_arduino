#include <ros.h>
#include <std_msgs/Float32.h>

ros::NodeHandle IMU_NODE;
std_msgs::Float32 yaw_angle_msg;
ros::Publisher IMU_pub("yaw_angle", &yaw_angle_msg);

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <math.h>
#define BNO055_SAMPLERATE_DELAY_MS (100)
float thetaM;
float phiM;
float thetaFold=0;
float thetaFnew;
float phiFold=0;
float phiFnew;
 
float thetaG=0;
float phiG=0;
 
float theta;
float phi;
 
float thetaRad;
float phiRad;
 
float Xm;
float Ym;
float psi;
 
 
float dt;
unsigned long millisOld;
 
 
Adafruit_BNO055 myIMU = Adafruit_BNO055();
 
void setup() {
Serial.begin(115200);
myIMU.begin();
delay(1000);
millisOld=millis();
 IMU_NODE.initNode();
 IMU_NODE.advertise(IMU_pub);
}
 
void loop() {
uint8_t system, gyro, accel, mg = 0;
myIMU.getCalibration(&system, &gyro, &accel, &mg);
imu::Vector<3> acc =myIMU.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
imu::Vector<3> gyr =myIMU.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
imu::Vector<3> mag =myIMU.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
thetaM=-atan2(acc.x()/9.8,acc.z()/9.8)/2/3.141592654*360;
phiM=-atan2(acc.y()/9.8,acc.z()/9.8)/2/3.141592654*360;
phiFnew=.95*phiFold+.05*phiM;
thetaFnew=.95*thetaFold+.05*thetaM;
 
dt=(millis()-millisOld)/1000.;
millisOld=millis();
theta=(theta+gyr.y()*dt)*.95+thetaM*.05;
phi=(phi-gyr.x()*dt)*.95+ phiM*.05;
thetaG=thetaG+gyr.y()*dt;
phiG=phiG-gyr.x()*dt;
 
phiRad=phi/360*(2*3.14);
thetaRad=theta/360*(2*3.14);
 
Xm=mag.x()*cos(thetaRad)-mag.y()*sin(phiRad)*sin(thetaRad)+mag.z()*cos(phiRad)*sin(thetaRad);
Ym=mag.y()*cos(phiRad)+mag.z()*sin(phiRad);
 
psi=atan2(Ym,Xm)/(2*3.14)*360;

  yaw_angle_msg.data = psi;
  IMU_pub.publish(&yaw_angle_msg);
  IMU_NODE.spinOnce();
  
  phiFold=phiFnew;
  thetaFold=thetaFnew;
  
delay(BNO055_SAMPLERATE_DELAY_MS);
}
