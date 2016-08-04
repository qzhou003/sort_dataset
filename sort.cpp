#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <boost/algorithm/string.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio/videoio.hpp>

using namespace std;

typedef std::pair<string, float> Prediction;
typedef std::map<std::string,std::string> map_ss;
map_ss type_map;
typedef std::map<std::string,int> map_si;
map_si make_map;
// std::map<std::string,int> make_map
std::unordered_map<string,int> make_count;

vector<string> get_elements(const string path,string d){
	vector<string> res;
	return boost::split(res,path,boost::is_any_of("/"));
}

void init_type_map(){
	type_map["-1"]="uncertain";
    type_map["1"]="front";
    type_map["2"]="rear";
    type_map["3"]="side";
    type_map["4"]="front-side";
    type_map["5"]="rear-side";
}

void init_make_map(map_si &make_map){
	
}

void check_selected_makes_are_in_original_lists(vector<string> &make_lists,std::unordered_map<string,int> &make_count ){
    int found = 0;
    for (int i = 0;i<make_lists.size();++i){
        cout << make_lists[i] << " ";
        if(make_count.find(make_lists[i])!=make_count.end()){
//            cout << "is found! " << endl;
            found ++;
           make_count.find(make_lists[i]) -> second ++;
        }else cout << " is not found! " << endl;
    }
    cout << "found = " << found << endl;
    for(auto it=make_count.begin();it!=make_count.end();++it){
        if (it->second==0){
            cout << it->first << " is not found!" << endl;
        } else
            cout << it->first << " = " << it->second << endl;
    }
    cout << "Selected_makes = " << make_count.size() << "; Found = " << found << endl;
}

int main(int argc, char** argv){
    if(argc != 1) {
		std::cerr << "Usage: " << argv[0]
				  << " line 1"
				  << " line 2" << std::endl;
        return 0;
	}
    // *** init maps
    init_type_map();

//	string images_list = argv[1];
//    string images_list = "/home/admobilize/Qi/data/train_test_split/classification/test.txt";
    string images_list = "/Users/qizhou/Documents/Admobilize_detection/Dataset/HKcarData/train_test_split/classification/full_lists.txt";
    string data_folder = "/Users/qizhou/Documents/Admobilize_detection/Dataset/HKcarData/";
    string make_file = "/Users/qizhou/Documents/Admobilize_detection/Dataset/HKcarData/label_files/make_name_corrected.txt";
    string selected_make_file = "/Users/qizhou/Documents/Admobilize_detection/Dataset/HKcarData/label_files/make_name_selected_corrected.txt";

    
    string images_folder = data_folder + "/image/";
	string label_folder = data_folder + "/label/";
    string small_image_folder = data_folder + "/val_ROI/";


	string line;
    //*** get all make lists
	vector<string> make_lists;
	ifstream infile_make(make_file);
//    ofstream output_make("make_name_corrected.txt",ofstream::app);
	while(getline(infile_make,line)){
		make_lists.push_back(line);   // make name list
//        string to_write = line.substr(1,line.size()-2);
//        boost::trim(to_write);
//        output_make << to_write << endl;
	}
	//push make to be used into set
	ifstream selected_infile_make(selected_make_file);
//    ofstream output_selected("make_name_selected_corrected.txt",ofstream::app);
	while(getline(selected_infile_make,line)){
        make_count[line] = 0;
//        string to_write = line.substr(1,line.size()-2);
//        boost::trim(to_write);
//        output_selected << to_write << endl;
	}
//    return 0;
    cout << "There are " << make_count.size() << " makes selected and pushed into map; " << endl;

    //** test if all selected makes are found in the original make lists. This is done before converting
//    check_selected_makes_are_in_original_lists(make_lists,make_count);return 0;

	ifstream infile(images_list);
    ofstream ofs ("val_car_make.txt",ofstream::app);

    ofstream ofs_front("front.dat",ofstream::trunc);
    ofstream ofs_front_side("front-side.dat",ofstream::trunc);
    ofstream ofs_side("side.dat",ofstream::trunc);
    ofstream ofs_rear_side("rear-side.dat",ofstream::trunc);
    ofstream ofs_rear("rear.dat",ofstream::trunc);
    ofstream ofs_uncertain("uncertain.dat",ofstream::trunc);
    ofstream ofs_all("all.dat",ofstream::trunc);

	cv::Mat img;
    int line_count = 0;
    unordered_set<string> makes_in_all_txt_file;
    string current_make = "qi";
    ofstream ofs_problems("problem.txt",ofstream::trunc);
	while(getline(infile,line)){
		string image_path = images_folder + line;
		vector<string> line_elements;
        boost::split(line_elements,line,boost::is_any_of("/"));
        int make_code = stoi(line_elements[0]);
        string make_name_ori = make_lists[make_code-1]; // make_name "like BMW, Audi"
//        string make_name = make_name_ori.substr(1,make_name_ori.size()-2);
        string make_name = make_name_ori;
        boost::trim(make_name);
        if(make_name.size()!=make_name_ori.size()) {cout << "!!! Becareful some spaces are removed from make_name_ori in make_name" << endl;}
        string ori_img_name = line_elements.back();
        if(makes_in_all_txt_file.find(make_name)==makes_in_all_txt_file.end()) {makes_in_all_txt_file.insert(make_name);}

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
		int dir_code = stoi(vals[0]);  // *** code for direction ***

        vector<string> strs;
		boost::split(strs,vals[2],boost::is_any_of(" "));
		if(strs.size()!=4) {std::cout << "Bounding box coordinates mistake. skipping this one." << std::endl;continue;}
		cv::Point LT(stoi(strs[0]),stoi(strs[1]));cv::Point RB(stoi(strs[2]),stoi(strs[3])); // *** coordinates for bounding box ***
		
        if(LT.x<0 || LT.y<0 || RB.x>img.cols || RB.y>img.rows || LT.x > img.cols || LT.y>img.rows || RB.x <0 || RB.y<0){
            cout << "******* LT or RB are out of boundary. ******" << endl;
            ofs_problems << line << " " << vals[2]  << endl;
            continue;
        }
        
		// ************  modify following to arrange images  ********************
        if(current_make != make_name_ori) {
            current_make = make_name_ori;
            cout << "Current_make = " << current_make << endl;
        }
        if(make_count.find(make_name_ori) != make_count.end()){
            if(make_count[make_name_ori] ++ == 0) {cout << "Trying to write into a new make: "  << make_name << endl;}
        } else continue;
        if(line_count++ %1000==0) {cout << "Progress: " << line_count << endl;}
        // ************ arrange by direction: direction/images
        string ROI_img_folder = "/Users/qizhou/Documents/Admobilize_detection/Dataset/HKcarData/qi/by_dir/";
        string ROI_img_name = type_map[to_string(dir_code)] + "/" + make_name + "/"+ to_string(make_code)+ "_" + make_name + "_" + type_map[to_string(dir_code)] + "_"  +ori_img_name;
        string ROI_img_path = ROI_img_folder + ROI_img_name;
        cv::Mat ROI_img = cv::Mat(img,cv::Rect(LT,RB));
        bool status = cv::imwrite(ROI_img_path, ROI_img);
        if(!status) {cout << "ROI_img_path = " << ROI_img_path << " is failded." << endl;}
        switch(dir_code){
        case 1:
            ofs_front << ROI_img_name << " " << 1 << " " << 0 << " " << 0 << " " << ROI_img.cols << " " << ROI_img.rows << endl; // opencv data format
            break;
        case 4:
            ofs_front_side << ROI_img_name << " " << 1 << " " << 0 << " " << 0 << " " << ROI_img.cols << " " << ROI_img.rows << endl; // opencv data format
            break;
        case 2:
            ofs_rear << ROI_img_name << " " << 1 << " " << 0 << " " << 0 << " " << ROI_img.cols << " " << ROI_img.rows << endl; // opencv data format
            break;
        case 5:
            ofs_rear_side << ROI_img_name << " " << 1 << " " << 0 << " " << 0 << " " << ROI_img.cols << " " << ROI_img.rows << endl; // opencv data format
            break;
        case 3:
            ofs_side << ROI_img_name << " " << 1 << " " << 0 << " " << 0 << " " << ROI_img.cols << " " << ROI_img.rows << endl; // opencv data format
            break;
        case -1:
            ofs_uncertain << ROI_img_name << " " << 1 << " " << 0 << " " << 0 << " " << ROI_img.cols << " " << ROI_img.rows << endl; // opencv data format
            break;
        }
        ofs_all << ROI_img_name << " " << 1 << " " << 0 << " " << 0 << " " << ROI_img.cols << " " << ROI_img.rows << endl; // opencv data format


        // *********** arrange by selected make: direction/make/images


        
        
        
        /*
        string small_img_name = to_string(make_code) + "/" + make_name + "_" + type_map[to_string(dir_code)] + "_" +ori_img_name;
        cout << "small_img_name = " << small_img_name << endl;
        cv::Mat ROI_img = cv::Mat(img,cv::Rect(LT,RB));
        ofstream ofs ("train_car_make.txt",ofstream::app);
        ofs << small_img_name << " " << make_code << endl;
        cv::imwrite(small_image_folder+small_img_name,ROI_img);
         */

        // putText(img,make_name,cv::Point(img.cols-400,100),cv::FONT_HERSHEY_PLAIN,5,cvScalar(0,0,255),3,CV_AA);
		// rectangle(img,LT,RB,cv::Scalar(0,255,0),3);
		// imshow("window",img);
        int key = cv::waitKey(2);
	}
    cout << "There are " << line_count << " lines processed..." << endl;
    int total = 0;
    for (auto it = make_count.begin(); it != make_count.end();++it){
        cout << it->first << " : " << it->second << endl;
        total += it-> second;
    }
    for(auto it = makes_in_all_txt_file.begin();it!=makes_in_all_txt_file.end();++it){
        cout << *it << endl;
    }
//    ofs_dir.close();
//    ofs_dir_dat.close();
    cout << "There are total " << total << "images in the set " <<endl;

}
