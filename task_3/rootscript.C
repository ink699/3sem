void draw_graph()
{
  Double_t ex_time[100];
  Double_t ex_time_err[100];
  Double_t thr_num[100];
  Double_t thr_num_err[100] = {0.0};
  ifstream in("stat_copy.txt");
  TCanvas *MyC = new TCanvas("MyC","",1000, 1000);
  MyC->SetGrid();
  TH1F *hr = MyC->DrawFrame(0.0, 30, 100.0, 200.0);
  hr->SetXTitle("Number of threads");
  hr->SetYTitle("Time, s");
  TF1* fit_func1 = new TF1("fit_func1", "[0]/x + [1] * x + [2]", 0, 100);
  for(Int_t i = 0; i < 100; ++i)
  {
    in >> thr_num[i] >> ex_time[i] >> ex_time[i];
    ex_time_err[i] = 0.5;
  }
  TGraph *gr1 = new TGraphErrors(100,thr_num,ex_time,thr_num_err,ex_time_err);
  gr1->SetLineColor(4);
  gr1->SetLineWidth(1);
  gr1->SetMarkerColor(4);
  gr1->SetMarkerStyle(20);
  gr1->Fit("fit_func1", "R");
  gr1->Draw("P");
}
