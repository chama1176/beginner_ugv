#include <ros/ros.h>
#include <ros/console.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <termios.h>
#include <stdio.h>
#include <string>
#include "sensor_msgs/Joy.h"

std::string dev("/dev/serial/by-id/usb-Silicon_Labs_CP2102_USB_to_UART_Bridge_Controller_0001-if00-port0");
//std::string dev("/dev/ttyUSB0");
boost::asio::io_service io;
boost::asio::streambuf sb;
boost::asio::serial_port port(io, dev);
//std::string send_data;
enum flush_type
{
  flush_receive = TCIFLUSH,
  flush_send = TCOFLUSH,
  flush_both = TCIOFLUSH
};
void flush_serial_port(
  boost::asio::serial_port& serial_port,
  flush_type what,
  boost::system::error_code& error)
{
  if (0 == ::tcflush(serial_port.lowest_layer().native_handle(), what))
  {
    error = boost::system::error_code();
  }
  else
  {
    error = boost::system::error_code(errno,
        boost::asio::error::get_system_category());
  }
}

void joy_callback(const sensor_msgs::Joy& joy_msg){
	boost::system::error_code ec;
	int vel[2] = {};

    vel[0] =joy_msg.axes[1] * 100;
    vel[1] =joy_msg.axes[4] * 100;
	
	std::string send_data = std::to_string(vel[0]) + "," + std::to_string(vel[1]) + "\r";  
	//std::cout << std::to_string(vel[0]) << "," << std::to_string(vel[1]) << std::endl;
	ROS_INFO("%s",send_data.c_str());
	boost::asio::write(port, boost::asio::buffer(send_data));

	flush_serial_port(port, flush_both, ec);
	boost::asio::read_until(port, sb, "\r\n");
	std::istream is(&sb);
	std::string rxed;
	std::getline(is, rxed);
	std::vector<std::string> rx_vals;
	boost::algorithm::split(rx_vals, rxed, boost::is_any_of(","));
	ROS_INFO("%d %s", (int)rx_vals.size(), rxed.c_str());
	//while(!ec){
	//	boost::asio::read(port, sb, boost::asio::transfer_at_least(1));	
	//}
	//int len = port.read_some(boost::asio::buffer(sb));
	//sb[len] = '\0';]
	
}

int main(int argc, char **argv){

//	std::string dev("/dev/serial/by-id/usb-Silicon_Labs_CP2102_USB_to_UART_Bridge_Controller_0001-if00-port0");
	long baud(57600);

	ros::init(argc, argv, "psoc_ros_serial");
	ros::NodeHandle nh;

	 //subscriibe
    ros::Subscriber joy_sub   = nh.subscribe("joy", 1, joy_callback);

	ros::Rate rate(10);
	int seq_cnt(0);
	int i;

//	boost::asio::io_service io;
//	boost::asio::serial_port port(io, dev);
	port.set_option(boost::asio::serial_port_base::baud_rate(baud));
	port.set_option(boost::asio::serial_port_base::character_size(8));
	port.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
	port.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
	port.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));

	ROS_INFO("psoc ros serial started");

	boost::asio::streambuf sb;
	while(ros::ok()){

/*
		boost::asio::read_until(port, sb, "\r\n");
		std::istream is(&sb);
		std::string rxed;
		std::getline(is, rxed);
		//std::string rxed = boost::asio::buffer_cast<const char*>(sb.data());
		std::vector<std::string> rx_vals;
		boost::algorithm::split(rx_vals, rxed, boost::is_any_of(","));
		ROS_INFO("%d %s", (int)rx_vals.size(), rxed.c_str());
*/
/*
		for(i=0;i<=rxed.size();i++){
			printf("%x ",rxed[i]);
		//	printf("%x \r\n",rxed.c_str());
		}
		printf("\r\n");
*/
/*
		if(rx_vals.size()%5 == 0){
			try{
				kir_cntl_msg.length = boost::lexical_cast<double>(rx_vals[0]);
				kir_cntl_msg.tension = boost::lexical_cast<double>(rx_vals[1]);
				kir_cntl_msg.pitch = boost::lexical_cast<double>(rx_vals[2]);
				kir_cntl_msg.yaw = boost::lexical_cast<double>(rx_vals[3]);

				kir_cntl_ros_pub.publish(kir_cntl_msg);
			}
			catch(const boost::bad_lexical_cast &){
				ROS_INFO("lexical_cast error");
				continue;
			}
		}else{
			ROS_INFO("Received string is not fit to format!!");
			ROS_INFO("%d %s", (int)rx_vals.size(), rxed.c_str());
			continue;
		}
*/	

		ros::spinOnce();
		rate.sleep();	
	}

	return 0;
}
