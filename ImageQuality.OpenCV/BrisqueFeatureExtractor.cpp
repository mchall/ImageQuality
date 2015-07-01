#include "Stdafx.h"
#include "BrisqueFeatureExtractor.h"

using namespace std;
using namespace cv;

namespace ImageQuality
{
	IList<double>^ BrisqueFeatureExtractor::BrisqueFeatures(array<uchar>^ buffer)
	{
		pin_ptr<uchar> p = &buffer[0];
		Mat data(1, buffer->Length, CV_8U, (void*)p, CV_AUTO_STEP);
		Mat image = imdecode(data, CV_LOAD_IMAGE_GRAYSCALE);

		int newHeight = (256 / double(image.cols)) * image.rows;
		resize(image, image, Size(256, newHeight));

		vector<double> feat = brisquefeatures(image);

		List<double>^ list = gcnew List<double>(5);
		for (uint i = 0; i < feat.size(); i++)
		{
			list->Add(feat[i]);
		}
		return list;
	}

	vector<double> BrisqueFeatureExtractor::brisquefeatures(Mat image)
	{
		image.convertTo(image, CV_64FC1);//char2double
		vector<double> feat;
		brisque_feature(image, feat);
		return feat;
	}

	void BrisqueFeatureExtractor::brisque_feature(Mat imdist, vector<double> &feat)
	{
		if (imdist.empty())
		{
			cout << "img read fail" << endl;
		}
		//------------------------------------------------
		//Feature Computation
		//-------------------------------------------------

		int scalenum = 2;
		feat.clear();
		Mat imdisthalf = Mat::zeros(imdist.rows / 2, imdist.cols / 2, imdist.type());
		if (2 == scalenum)
		{
			brisque_process(imdist, feat);
			resize(imdist, imdisthalf, Size(imdist.cols / 2, imdist.rows / 2));
			brisque_process(imdisthalf, feat);
		}
	}

	void BrisqueFeatureExtractor::estimateggdparam(Mat vec, double &gamparam, double &sigma)
	{
		Mat vec2 = vec.clone();
		Scalar sigma_sq = mean(vec2.mul(vec2));
		sigma = sqrt(sigma_sq[0]);
		Scalar E = mean(abs(vec));
		double rho = sigma_sq[0] / (E[0] * E[0]);

		vector<double> gam;
		vector<double> r_gam;
		vector<double> rho_r_gam;
		uint number = int((10 - 0.2f) / 0.001f) + 1;
		gam.clear();
		r_gam.clear();
		rho_r_gam.clear();
		gam.resize(number);
		r_gam.resize(number);
		rho_r_gam.resize(number);

		for (uint i = 0; i < number; i++)
		{
			if (0 == i)
			{
				gam[i] = 0.2;
			}
			else
			{
				gam[i] = gam[i - 1] + 0.001f;
			}
			r_gam[i] = (Gamma(1.f / gam[i])*Gamma(3.f / gam[i])) / (Gamma(2. / gam[i])*Gamma(2. / gam[i]));
			rho_r_gam[i] = abs(rho - r_gam[i]);
		}

		int pos = (int)(min_element(rho_r_gam.begin(), rho_r_gam.end()) - rho_r_gam.begin());
		//gamma
		gamparam = gam[pos];
	}

	void BrisqueFeatureExtractor::estimateaggdparam(Mat vec, double &alpha, double &leftstd, double &rightstd)
	{
		vector<double> left;
		vector<double> right;
		left.clear();
		right.clear();
		for (uint i = 0; i < vec.rows; i++)
		{
			double *data1 = vec.ptr<double>(i);
			for (uint j = 0; j < vec.cols; j++)
			{
				if (data1[j] < 0)
				{
					left.push_back(data1[j]);
				}
				else if (data1[j] > 0)
				{
					right.push_back(data1[j]);
				}
			}
		}
		for (uint i = 0; i < left.size(); i++)
		{
			left[i] = left[i] * left[i];
		}
		for (uint i = 0; i < right.size(); i++)
		{
			right[i] = right[i] * right[i];
		}
		double leftsum = 0.f;
		for (uint i = 0; i < left.size(); i++)
		{
			leftsum += left[i];
		}
		double rightsum = 0.f;
		for (uint i = 0; i < right.size(); i++)
		{
			rightsum += right[i];
		}

		leftstd = sqrt(leftsum / left.size());//mean
		rightstd = sqrt(rightsum / right.size());//mean
		double gammahat = leftstd / rightstd;
		Mat vec2;
		multiply(vec, vec, vec2);
		Scalar tmp1 = mean(abs(vec));
		Scalar tmp2 = mean(vec2);
		double rhat = tmp1[0] * tmp1[0] / tmp2[0];

		double rhatnorm = (rhat*(gammahat*gammahat*gammahat + 1)*(gammahat + 1)) / ((gammahat*gammahat + 1)*(gammahat*gammahat + 1));

		vector<double> gam;
		vector<double> r_gam;
		vector<double> r_gam_rha;
		uint number = int((10 - 0.2f) / 0.001f) + 1;
		gam.resize(number);
		r_gam.resize(number);
		r_gam_rha.resize(number);

		for (uint i = 0; i < number; i++)
		{
			if (0 == i)
			{
				gam[0] = 0.2;
			}
			else
			{
				gam[i] = gam[i - 1] + 0.001f;
			}

			r_gam[i] = (Gamma(2.f / gam[i])*Gamma(2.f / gam[i])) / (Gamma(1. / gam[i])*Gamma(3. / gam[i]));
			r_gam_rha[i] = (r_gam[i] - rhatnorm)*(r_gam[i] - rhatnorm);
		}


		//find min and pos
		int pos = (int)(min_element(r_gam_rha.begin(), r_gam_rha.end()) - r_gam_rha.begin());
		alpha = gam[pos];
	}

	void BrisqueFeatureExtractor::brisque_process(Mat imdist, vector<double> &feat)
	{
		if (imdist.empty())
		{
			cout << "img read fail" << endl;
		}
		Mat mu, mu_sq;
		Mat sigma = Mat::zeros(imdist.rows, imdist.cols, imdist.type());
		Mat imgdouble;

		Mat imdist_mu;
		Mat avoidzero;
		double alpha, overallstd;
		Mat structdis;

		GaussianBlur(imdist, mu, Size(7, 7), 7.f / 6, 7.f / 6, 0);
		multiply(mu, mu, mu_sq);
		multiply(imdist, imdist, imgdouble);

		GaussianBlur(imgdouble, imgdouble, Size(7, 7), 7.f / 6, 7.f / 6, 0);

		for (uint i = 0; i < imgdouble.rows; i++)
		{
			double *data1 = imgdouble.ptr<double>(i);
			double *data2 = mu_sq.ptr<double>(i);
			double *data3 = sigma.ptr<double>(i);
			for (uint j = 0; j < imgdouble.cols; j++)
			{
				data3[j] = sqrt(abs(data1[j] - data2[j]));
			}
		}

		subtract(imdist, mu, imdist_mu);
		avoidzero = Mat::ones(sigma.rows, sigma.cols, sigma.type());
		add(sigma, avoidzero, sigma);

		divide(imdist_mu, sigma, structdis);

		estimateggdparam(structdis, alpha, overallstd);

		feat.push_back(alpha);

		feat.push_back(overallstd*overallstd);

		Mat shifted_structdis;
		Mat pair;
		double constvalue, meanparam, leftstd, rightstd;
		int shifts[][2] = { 0, 1, 1, 0, 1, 1, -1, 1 };

		for (uint itr_shift = 0; itr_shift < 4; itr_shift++)
		{
			shifted_structdis = circshift(structdis, shifts[itr_shift][0], shifts[itr_shift][1]);
			pair = structdis.mul(shifted_structdis);
			estimateaggdparam(pair,
				alpha,
				leftstd,//left sigma
				rightstd//right sigma
				);

			constvalue = (sqrt(Gamma(1 / alpha)) / sqrt(Gamma(3 / alpha)));
			meanparam = (rightstd - leftstd)*(Gamma(2 / alpha) / Gamma(1 / alpha))*constvalue;

			feat.push_back(alpha);
			feat.push_back(meanparam);
			feat.push_back(leftstd*leftstd);
			feat.push_back(rightstd*rightstd);
		}
	}

	Mat BrisqueFeatureExtractor::circshift(Mat structdis, int a, int b)
	{
		Mat shiftx = Mat::zeros(structdis.rows, structdis.cols, structdis.type());
		if (0 == a)
		{//unchanged 
			shiftx = structdis.clone();
		}
		else if (1 == a)
		{//		
			for (uint i = 0; i < structdis.rows - 1; i++)
			{
				for (uint j = 0; j < structdis.cols; j++)
				{
					shiftx.at<double>(i + 1, j) = structdis.at<double>(i, j);
				}
			}
			for (uint j = 0; j < structdis.cols; j++)
				shiftx.at<double>(0, j) = structdis.at<double>(structdis.rows - 1, j);
		}
		else if (-1 == a)
		{
			for (uint i = 0; i < structdis.rows - 1; i++)
			{
				for (uint j = 0; j < structdis.cols; j++)
				{
					shiftx.at<double>(i, j) = structdis.at<double>(i + 1, j);
				}
			}
			for (uint j = 0; j < structdis.cols; j++)
				shiftx.at<double>(structdis.rows - 1, j) = structdis.at<double>(0, j);
		}
		Mat shifty = Mat::zeros(shiftx.rows, shiftx.cols, shiftx.type());
		if (0 == b)
		{
			shifty = shiftx.clone();
		}
		else if (1 == b)
		{
			for (uint i = 0; i < shiftx.rows; i++)
			{
				for (uint j = 0; j < shiftx.cols - 1; j++)
				{
					shifty.at<double>(i, j + 1) = shiftx.at<double>(i, j);
				}
			}
			for (uint i = 0; i < shiftx.rows; i++)
				shifty.at<double>(i, 0) = shiftx.at<double>(i, shiftx.cols - 1);
		}


		return shifty;
	}

	double BrisqueFeatureExtractor::Gamma(double x)
	{//x>0
		if (x > 2 && x <= 3)
		{
			const double c0 = 0.0000677106;
			const double c1 = -0.0003442342;
			const double c2 = 0.0015397681;
			const double c3 = -0.0024467480;
			const double c4 = 0.0109736958;
			const double c5 = -0.0002109075;
			const double c6 = 0.0742379071;
			const double c7 = 0.0815782188;
			const double c8 = 0.4118402518;
			const double c9 = 0.4227843370;
			const double c10 = 1.0000000000;
			double temp = 0;
			temp = temp + c0*pow(x - 2.0, 10.0) + c1*pow(x - 2.0, 9.0);
			temp = temp + c2*pow(x - 2.0, 8.0) + c3*pow(x - 2.0, 7.0);
			temp = temp + c4*pow(x - 2.0, 6.0) + c5*pow(x - 2.0, 5.0);
			temp = temp + c6*pow(x - 2.0, 4.0) + c7*pow(x - 2.0, 3.0);
			temp = temp + c8*pow(x - 2.0, 2.0) + c9*(x - 2.0) + c10;
			return temp;
		}
		else if (x > 0 && x <= 1)
		{
			return Gamma(x + 2) / (x*(x + 1));
		}
		else if (x > 1 && x <= 2)
		{
			return Gamma(x + 1) / x;
		}
		else if (x > 3)
		{
			int i = 1;
			double temp = 1;
			while (((x - i) > 2 && (x - i) <= 3) == false)
			{
				temp = (x - i) * temp;
				i++;
			}
			temp = temp*(x - i);
			return temp*Gamma(x - i);
		}
		else
		{
			return 0;
		}
	}
}