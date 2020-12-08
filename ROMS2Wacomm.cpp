//
// Created by Raffaele Montella on 07/12/20.
//

#include "ROMS2Wacomm.hpp"

Array1<double>& ROMS2Wacomm::Depth() { return  depth; }
Array2<double>& ROMS2Wacomm::Zeta() { return  zeta; }
Array2<double>& ROMS2Wacomm::Lon() { return  lon; }
Array2<double>& ROMS2Wacomm::Lat() { return  lat; }

Array2<double>& ROMS2Wacomm::Mask() { return  mask_rho; }

Array4<double>& ROMS2Wacomm::U() { return  ucomp; }
Array4<double>& ROMS2Wacomm::V() { return vcomp; }
Array4<double>& ROMS2Wacomm::W() { return wcomp; }
Array4<double>& ROMS2Wacomm::AKT() { return aktcomp; }

ROMS2Wacomm::ROMS2Wacomm(string &fileName): fileName(fileName) {
    log4cplus::BasicConfigurator basicConfig;
    basicConfig.configure();
    logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("WaComM"));


}

void ROMS2Wacomm::process()
{
    LOG4CPLUS_INFO(logger,"Loading:"+fileName);

    // Open the file for read access
    netCDF::NcFile dataFile(fileName, NcFile::read);

    // Retrieve the variable named "depth"
    //NcVar varDepth=dataFile.getVar("depth");
    //size_t s_rho = varDepth.getDim(0).getSize();
    //depth.Allocate(s_rho);
    //varDepth.getVar(depth());

    // Retrieve the variable named "mask_rho"
    NcVar varMaskRho=dataFile.getVar("mask_rho");
    size_t eta_rho = varMaskRho.getDim(0).getSize();
    size_t xi_rho = varMaskRho.getDim(1).getSize();
    mask_rho.Allocate(eta_rho,xi_rho);
    varMaskRho.getVar(mask_rho());

    // Retrieve the variable named "mask_u"
    NcVar varMaskU=dataFile.getVar("mask_u");
    size_t eta_u = varMaskU.getDim(0).getSize();
    size_t xi_u = varMaskU.getDim(1).getSize();
    Array2<double> mask_u(eta_u,xi_u);
    varMaskU.getVar(mask_u());

    // Retrieve the variable named "mask_v"
    NcVar varMaskV=dataFile.getVar("mask_v");
    size_t eta_v = varMaskV.getDim(0).getSize();
    size_t xi_v = varMaskV.getDim(1).getSize();
    Array2<double> mask_v(eta_v,xi_v);
    varMaskV.getVar(mask_v());



    // Retrieve the variable named "lat_rho"
    NcVar varLatRho=dataFile.getVar("lat_rho");
    lat.Allocate(eta_rho,xi_rho);
    varLatRho.getVar(lat());

    // Retrieve the variable named "lon_rho"
    NcVar varLonRho=dataFile.getVar("lon_rho");
    lon.Allocate(eta_rho,xi_rho);
    varLonRho.getVar(lon());


    // Retrieve the variable named "zeta"
    NcVar varZeta=dataFile.getVar("zeta");
    zeta.Allocate(eta_rho,xi_rho);
    varMaskV.getVar(zeta());

    // Retrieve the variable named "u"
    NcVar varU=dataFile.getVar("u");
    size_t ocean_time = varU.getDim(0).getSize();
    size_t s_rho = varU.getDim(1).getSize();
    Array4<double> u(ocean_time, s_rho, eta_u, xi_u);
    varU.getVar(u());

    // Retrieve the variable named "v"
    NcVar varV=dataFile.getVar("v");
    Array4<double> v(ocean_time, s_rho, eta_v, xi_v);
    varV.getVar(v());

    // Retrieve the variable named "w"
    NcVar varW=dataFile.getVar("w");
    size_t s_w = varW.getDim(1).getSize();
    Array4<double> w(ocean_time,s_w,eta_rho,xi_rho);
    varV.getVar(w());

    // Retrieve the variable named "AKt"
    NcVar varAKt=dataFile.getVar("AKt");
    Array4<double> akt(ocean_time,s_w,eta_rho,xi_rho);
    varAKt.getVar(akt());

    ucomp.Allocate(ocean_time,s_rho,eta_rho,xi_rho);
    vcomp.Allocate(ocean_time,s_rho,eta_rho,xi_rho);
    wcomp.Allocate(ocean_time,s_w,eta_rho,xi_rho);
    aktcomp.Allocate(ocean_time,s_w,eta_rho,xi_rho);



    uv2rho(mask_u, mask_v, u,v);
    wakt2rho(mask_u, mask_v, w, akt);


}

void ROMS2Wacomm::uv2rho(Array2<double>& mask_u, Array2<double>& mask_v, Array4<double>& u, Array4<double>& v) {
    double uw1, uw2, vw1, vw2;

    LOG4CPLUS_INFO(logger,"uv2rho");

    size_t ocean_time = u.Nx();
    size_t s_rho = u.Ny();
    size_t eta_v = mask_v.Nx();
    size_t xi_v = mask_v.Ny();
    size_t eta_u = mask_v.Nx();
    size_t xi_u = mask_u.Ny();
    size_t eta_rho = mask_rho.Nx();
    size_t xi_rho = mask_rho.Ny();

    for (int t=0; t < ocean_time; t++)
    {
        for (int k=0; k < s_rho; k++)
        {
            for (int j=0; j < eta_v; j++)
            {
                for (int i=0; i< xi_u; i++)
                {

                    if ( j>=0 && i>=0 && j < eta_rho && i < xi_rho && mask_rho(j,i) > 0.0 )
                    {
                        if ( j>=0 && i>=0 && j<eta_u && i <xi_u && mask_u(j,i) > 0.0 )
                        {
                            uw1=u(t,k,j,i);
                        } else {
                            uw1=0.0;
                        }
                        if ( j>0 && i>=0 && j<eta_u && i <xi_u && mask_u(j-1,i) > 0.0 )
                        {
                            uw2=u(t,k,j-1,i);
                        } else
                        {
                            uw2=0.0;
                        }
                        if ( j>=0 && i>=0 && j<eta_v && i <xi_v && mask_v(j,i) > 0.0 )
                        {
                            vw1=v(t,k,j,i);
                        } else
                        {
                            vw1=0.0;
                        }
                        if ( j>=0 && i>0 && j<eta_v && i <xi_v && mask_v(j,i-1) > 0.0 )
                        {
                            vw2=v(t,k,j,i-1);
                        } else
                        {
                            vw2=0.0;
                        }

                        ucomp(t,k,j,i)=0.5*(uw1+uw2);
                        vcomp(t,k,j,i)=0.5*(vw1+vw2);
                    } else {
                        ucomp(t,k,j,i)=0.0;
                        vcomp(t,k,j,i)=0.0;
                    }
                }
            }
        }
    }
}

void ROMS2Wacomm::wakt2rho(Array2<double>& mask_u, Array2<double>& mask_v, Array4<double>& w, Array4<double>& akt ) {
    double ww1, ww2, ww3, aktw1, aktw2, aktw3;

    LOG4CPLUS_INFO(logger,"wakt2rho");

    size_t ocean_time = w.Nx();
    size_t s_w = w.Ny();
    size_t eta_v = mask_v.Nx();
    size_t xi_u = mask_u.Ny();
    size_t eta_rho = mask_rho.Nx();
    size_t xi_rho = mask_rho.Ny();


    for (int t=0; t < ocean_time; t++) {
        for (int k=0; k < s_w; k++) {
            for (int j=0; j < eta_v; j++) {
                for (int i=0; i< xi_u; i++) {

                    if ( j>=0 && i>=0 && j<eta_rho && i <xi_rho && mask_rho(j,i) > 0.0 )
                    {
                        if ( j>=0 && i>0 && j<eta_rho && i <xi_rho && mask_rho(j,i-1) > 0.0 )
                        {
                            ww1=w(t, k,j,i-1);
                            aktw1=akt(t, k,j,i-1);
                        } else {
                            ww1=0.0;
                            aktw1=0.0;
                        }

                        if ( j>0 && i>=0 && j<eta_rho && i <xi_rho && mask_rho(j-1,i) > 0.0 ) {
                            ww2=w(t, k,j-1,i);
                            aktw2=akt(t, k,j-1,i);
                        } else {
                            ww2=0.0;
                            aktw2=0.0;
                        }

                        if ( j>0 && i>0 && j<eta_rho && i <xi_rho && mask_rho(j-1,i-1) > 0.0 ) {
                            ww3=w(t, k, j-1,i-1);
                            aktw3=akt(t, k, j-1,i-1);
                        } else {
                            ww3=0.0;
                            aktw3=0.0;
                        }

                        wcomp(t,k,j,i)=0.25*(ww1+ww2+ww3+w(t,k,j,i));
                        aktcomp(t,k,j,i)=0.25*(aktw1+aktw2+aktw3+akt(t,k,j,i));

                    } else {

                        wcomp(t,k,j,i)=0.0;
                        aktcomp(t,k,j,i)=0.0;
                    }
                }
            }
        }
    }
}

ROMS2Wacomm::~ROMS2Wacomm() {

}




