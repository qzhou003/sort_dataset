#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <boost/algorithm/string.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio/videoio.hpp>

using namespace std;

typedef std::pair<string, float> Prediction;
std::map<std::string,std::string> type_map;


vector<string> get_elements(const string path,string d){
	vector<string> res;
	return boost::split(res,path,boost::is_any_of("/"));
}

int main(int argc, char** argv){
    if(argc != 1) {
		std::cerr << "Usage: " << argv[0]
				  << " line 1"
				  << " line 2" << std::endl;
        return 0;
	}

//	string images_list = argv[1];
    string images_list = "/home/admobilize/Qi/data/train_test_split/classification/test.txt";
	string data_folder = "/home/admobilize/Qi/data/";
	string make_file = "/home/admobilize/Qi/data/label_files/make_name.txt";
    
    string images_folder = data_folder + "/image/";
	string label_folder = data_folder + "/label/";
    string small_image_folder = data_folder + "/val_ROI/";
    
    type_map["-1"]="uncertain";
    type_map["1"]="front";
    type_map["2"]="rear";
    type_map["3"]="side";
    type_map["4"]="front-side";
    type_map["5"]="rear-side";

	string line;
	vector<string> make_lists;
	ifstream infile_make(make_file);
	while(getline(infile_make,line)){
		make_lists.push_back(line);   // make name list
	}

	ifstream infile(images_list);
    ofstream ofs ("val_car_make.txt",ofstream::app);
	cv::Mat img;
	while(getline(infile,line)){
		string image_path = images_folder + line;
		vector<string> line_elements;
        boost::split(line_elements,line,boost::is_any_of("/"));
		int make_code = stoi(line_elements[0]);
		string make_name = make_lists[make_code-1];
        make_name = make_name.substr(1,make_name.size()-2);
        string ori_img_name = line_elements.back();

		img = cv::imread(image_path);
		string label_file = label_folder + line;
		label_file = label_file.substr(0,image_path.size()-3) + "txt";
		ifstream infile_label(label_file);
		string label_element;
		vector<string> vals;
		while(std::getline(infile_label,label_element)){
			vals.push_back(label_element);
		}
		if(vals.size()!=3) {std::cout << "Label elements reading failed. continue running..." << std::endl;continue;}
		int dir_code = stoi(vals[0]);  // code for direction

		vector<string> strs;
		boost::split(strs,vals[2],boost::is_any_of(" "));
		if(strs.size()!=4) {std::cout << "Bounding box coordinates mistake. skipping this one." << std::endl;continue;}
		cv::Point LT(stoi(strs[0]),stoi(strs[1]));cv::Point RB(stoi(strs[2]),stoi(strs[3]));
	    string small_img_name = to_string(make_code) + "/" + make_name+"_" + type_map[to_string(dir_code)] + "_" +ori_img_name;
	    cout << "small_img_name = " << small_img_name << endl;
        cv::Mat ROI_img = cv::Mat(img,cv::Rect(LT,RB));
        ofstream ofs ("train_car_make.txt",ofstream::app);
        ofs << small_img_name << " " << make_code << endl;
         //imshow("small_img",ROI_img);
         cv::imwrite(small_image_folder+small_img_name,ROI_img);
        // putText(img,make_name,cv::Point(img.cols-400,100),cv::FONT_HERSHEY_PLAIN,5,cvScalar(0,0,255),3,CV_AA);

		// rectangle(img,LT,RB,cv::Scalar(0,255,0),3);
		// imshow("window",img);
        int key = cv::waitKey(20);
	}
}
