#include "openfv.h"

using namespace cv;
using namespace std;

DEFINE_bool(fhelp, false, "show config file options");
DEFINE_bool(show_imgs, false, "show images");
DEFINE_bool(benchmark_mode, true, "benchmark mode");
DEFINE_bool(int_mode, false, "integer image mode");

DEFINE_string(config_file, "/home/ab9/projects/hf_data/config.cfg", "config file");
DEFINE_string(filename, "temp.jpg", "name of file to save image to");

DEFINE_double(zmin, 0, "zmin");
DEFINE_double(zmax, 100, "zmax");
DEFINE_double(dz, 0.1, "dz");
DEFINE_double(thresh, 0, "thresholding level");

DEFINE_int32(iters, 5, "number of times to evaluate");
DEFINE_int32(run_case, 0, "case to run");

int main(int argc, char** argv) {

    google::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr=1;

    refocus_settings settings;
    parse_refocus_settings(FLAGS_config_file, settings, FLAGS_fhelp);

    double t1, t2, t3, t4;
    vector<Mat> stack1, stack2, stack3, stack4;

    if (FLAGS_run_case==0 || FLAGS_run_case==1) {

        LOG(INFO)<<"Benchmarking with GPU on and HF on...";
        settings.use_gpu = 1;
        settings.hf_method = 1;
        double total = 0;
        for (int i=0; i<FLAGS_iters; i++) {
            saRefocus refocus(settings);
            if (FLAGS_benchmark_mode)
                refocus.setBenchmarkMode(1);
            if (FLAGS_int_mode)
                refocus.setIntImgMode(1);
            refocus.return_stack(0, 0.9, FLAGS_dz, FLAGS_thresh, 0, stack1, t1);
            total += t1;
            if (FLAGS_show_imgs)
                Movie mov(stack1);
        }
        t1 = total/FLAGS_iters;

    }

    if (FLAGS_run_case==0 || FLAGS_run_case==2) {

        LOG(INFO)<<"Benchmarking with GPU off and HF on...";
        settings.use_gpu = 0;
        settings.hf_method = 1;
        {
            saRefocus refocus(settings);
            if (FLAGS_benchmark_mode)
                refocus.setBenchmarkMode(1);
            if (FLAGS_int_mode)
                refocus.setIntImgMode(1);
            refocus.return_stack(0, 0.9, FLAGS_dz, FLAGS_thresh, 0, stack2, t2);
            if (FLAGS_show_imgs)
                Movie mov(stack2);
        }

    }

    if (FLAGS_run_case==0 || FLAGS_run_case==3) {

        LOG(INFO)<<"Benchmarking with GPU on and HF off...";
        settings.use_gpu = 1;
        settings.hf_method = 0;
        {
            saRefocus refocus(settings);
            if (FLAGS_benchmark_mode)
                refocus.setBenchmarkMode(1);
            if (FLAGS_int_mode)
                refocus.setIntImgMode(1);    
            refocus.return_stack(0, 0.9, FLAGS_dz, FLAGS_thresh, 0, stack3, t3);
            if (FLAGS_show_imgs)
                Movie mov(stack3);
        }

    }

    if (FLAGS_run_case==0 || FLAGS_run_case==4) {

        LOG(INFO)<<"Benchmarking with GPU off and HF off...";
        settings.use_gpu = 0;
        settings.hf_method = 0;
        {
            saRefocus refocus(settings);
            if (FLAGS_benchmark_mode)
                refocus.setBenchmarkMode(1);
            if (FLAGS_int_mode)
                refocus.setIntImgMode(1);
            refocus.return_stack(0, 0.01, FLAGS_dz, FLAGS_thresh, 0, stack4, t4);
            if (FLAGS_show_imgs)
                Movie mov(stack4);
        }

    }

    if (FLAGS_run_case==0) {

        LOG(INFO)<<"\t\t\t"<<"Original Method"<<"\t"<<"HF Method";
        LOG(INFO)<<"CPU only:"<<"\t\t"<<(t4*1000/60)<<" mins"<<"\t"<<t2*100<<" s";
        LOG(INFO)<<"CPU + GPU:"<<"\t"<<(t3*100)<<" s"<<"\t"<<(t1*100)<<" s";

    }

    vector<int> params;
    params.push_back(CV_IMWRITE_JPEG_QUALITY);
    params.push_back(100);
    imwrite(FLAGS_filename, stack4[0]*255, params);

    // ProfilerStart("bench.prof");    

    // ProfilerStop();

    return 0;

}
