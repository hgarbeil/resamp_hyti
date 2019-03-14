
# coding: utf-8

# In[1]:


import numpy as np
import matplotlib.pyplot as plt
#get_ipython().magic(u'matplotlib inline')
from astropy.io import ascii
from astropy.table import Table

#Colorblind friendly color scheme for graphs
CB_color_cycle = ['#377eb8', '#ff7f00', '#4daf4a',
                  '#f781bf', '#a65628', '#984ea3',
                  '#999999', '#e41a1c', '#dede00']


# In[2]:


def SIGNAL(w, T_c, dc_0, A, RN, QE, f_no, tau, band, mod):
    wave = w*(10**(-6)) #wavelength [meters]
    bandpass = (band*100)*(wave)**2 #bandpass in m

    #Calculate temperature in Kelvin
    T = T_c + 273.15 #[K]

    #Calculate spectral radiance in wavelengths
    L_lambda = (1.191*10**(-16))/((wave**5)*(np.exp((0.01438)/(wave*T))-1)) #[Watts/m^3/sr]

    #Caculate energy of a photon
    E = (1.99*10**(-25))/wave
    
    #Calculate spectral radiance in photons
    L_ph = (L_lambda*bandpass)/E  #[photons/s-m^2-sr]

    #Calculate Signal and Dark Current (without time)
    signal = QE*L_ph*A*(np.pi/(4*(f_no**2)))*tau*mod #[e-/s]
    
    return(signal)

def SIGNAL_T(w, T_c, dc_0, A, RN, QE, f_no, tau, band):
    #Calculate spectral resolution in wavelengths
    wave = w*(10**(-6)) #wavelength [meters]
    bandpass = band*(10**(-6)) #bandpass in [meters]

    #Calculate temperature in Kelvin
    T = T_c + 273.15 #[K]

    #Calculate spectral radiance in wavelengths
    L_lambda = (1.191*10**(-16))/((wave**5)*(np.exp((0.01438)/(wave*T))-1)) #[Watts/m^3/sr]
    
    #Caculate energy of a photon
    E = (1.99*10**(-25))/wave
    
    #Calculate spectral radiance in photons
    L_ph = (L_lambda*bandpass)/E  #[photons/s-m^2-sr]

    #Calculate Signal and Dark Current (without time)
    signal = QE*L_ph*A*(np.pi/(4*(f_no**2)))*tau #[e-/s]
    
    return(signal)

def thermal(w, T_c, A, QE, f_no, tau_th, band, emissivity):
    #Calculate spectral resolution in wavelengths
    wave = w*(10**(-6)) #wavelength [meters]
    bandpass = band*(10**(-6)) #bandpass in [meters]

    #Calculate temperature in Kelvin
    T = T_c + 273.15 #[K]

    #Calculate spectral radiance in wavelengths
    L_lambda = (1.191*10**(-16))/((wave**5)*(np.exp((0.01438)/(wave*T))-1)) #[Watts/m^3/sr]
    
    #Caculate energy of a photon
    E = (1.99*10**(-25))/wave
    
    #Calculate spectral radiance in photons
    L_ph = (L_lambda*bandpass)/E  #[photons/s-m^2-sr]

    #Calculate Signal and Dark Current (without time)
    thermal = emissivity*QE*L_ph*A*(np.pi/(4*(f_no**2)))*tau_th #[e-/s]
    
    return(thermal)

def NEdT(w, w1, w2, T_c, T_lens, fwc, dc_0, A, RN, QE, f_no, tau, band, mod, N):
    #calculate signal and dark current in e-/s
    signal = SIGNAL(w, T_c, dc_0, A, RN, QE, f_no, tau, band, mod)
    DC = (dc_0*A*10**4)/(1.602*10**(-19)) #[DC in e-/s] 

    #Calculate total signal from 8 to 11.5 micrometers
    waves=np.arange(w1,w2,intres)
    S_T=0
    for i in waves:
        S_T=S_T + SIGNAL_T(i, T_c, dc_0, A, RN, QE, f_no, tau, intres)
        
    TH=0
    for i in waves:
        TH=TH + thermal(i, T_lens, A, QE, f_no, tau_th, intres, emissivity)

    #Calculate time to 1/2 full well capacity
    t = (0.5*fwc)/(S_T + DC + TH) #time in seconds

    #Calculate Signal and Dark Current (with time)
    S_t = signal*t
    S_TOT = S_T*t
    DC_t = DC*t
    TH_t = TH*t

    #Calculate Noise
    NOISE = np.sqrt(S_TOT + DC_t + TH_t + RN**2)
    _N = np.sqrt(N)

    #Calculate Signal-to-Noise Ratio
    SNR = (S_t*_N)/NOISE
    
    T = T_c + 273.15 #[K]
    wave = w*(10**(-6)) #wavelength [meters]
    BB_measured = (1.191*10**(-16))/((wave**5)*(np.exp((0.01438)/(wave*T))-1))
    BB_theoretical = (1.191*10**(-16))/((wave**5)*(np.exp((0.01438)/(wave*(T+1)))-1))
    
    NEdT = (BB_measured/SNR)*(1/(BB_theoretical - BB_measured))                    
    return(NEdT, SNR, DC_t, TH_t, t)


# In[3]:

### PerfModel main function
# This program steps through the 25 HyTI Wavebands starting at 8.019 microns and incrementing up to 10.7 microns (13 wavenumber spacing)
# At each waveband the SNR and NeDT are calculated at a 25C lens, for a target from 0 C to 50C at .1C increment
#
#Enter variables
outcoefs = np.zeros ((25,4),dtype=np.float32) #this will store the polynomial coefficents then write to file when fully loaded
for w_iter in reversed(range (25)):
    w1 = 8 #wavelength in micrometers
    w2 = 10.7
    w = 10000./ (935. + 13. * w_iter)


    #signal
    L = 24 #detector pixel length [micrometer]
    A = (L*10**(-6))**2 #detector size [m^2]
    QE = 0.3 #quantum efficiency at lambda = 10 micrometers
    #f_no = 3.823
    tau = 0.5
    #band = 10 #bandpass in wavenumbers [cm-1]
    mod = 0.4 #modulation efficiency
    T_c = np.linspace(0,50,500)

    #total integrated signal
    #intres = 0.00001 #in wavelengths units micrometers
    intres = .001
    #thermal
    T_lens_0 = 0
    T_lens_25 = 25
    T_lens_50 = 50
    tau_th = 0.5
    emissivity = 0.5
    #intres = 0.00001 #in wavelengths units micrometers

    #SNR
    N = 288 #magic number! (number of samples)
    RN = 500 #[e-]
    fwc = 8*10**6 #FWC in electrons
    dc_0 = 10**(-4) #Dark Current [A/cm^2]
    #F_max = 4*f_no**2+np.pi/4 #number of fringes
    #band_count = ((10000/w1) - (10000/w2))/band


    b25 = 13 #wavenumbers (for a bandcount of 25)
    f_25 = np.sqrt(((10000/w1)/(8*b25))-(np.pi/16))

    F_m25 = 4*f_25**2+np.pi/4 #number of fringes


    #SNR_0 = NEdT(w, w1, w2, T_c, T_lens_0, fwc, dc_0, A, RN, QE, f_25, tau, b25, mod, N)
    ### SNR_25 represents the SNR at a lens at 25C
    SNR_25 = NEdT(w, w1, w2, T_c, T_lens_25, fwc, dc_0, A, RN, QE, f_25, tau, b25, mod, N)
    #SNR_50 = NEdT(w, w1, w2, T_c, T_lens_50, fwc, dc_0, A, RN, QE, f_25, tau, b25, mod, N)


    # In[4]:

    fcoefs = np.polyfit (T_c, SNR_25[0], 3)
    outcoefs[24-w_iter,:]=fcoefs[::-1]
    fittedvals = np.poly1d(fcoefs)
    my0 = fittedvals(0.)
    my25 = fittedvals(25.)
    my50= fittedvals(50.)
    myfitted = fittedvals (T_c)
    ## for plotting, not here though
    #plt.plot (T_c, SNR_25[0], color = CB_color_cycle[0])
    #plt.plot (T_c, myfitted, color=CB_color_cycle[1], marker=1)
    #plt.plot(T_c, SNR_0[0], color = CB_color_cycle[0])
    #plt.plot(T_c, SNR_50[0], color = CB_color_cycle[2])
    #plt.xlabel(r'$T_{surface} \; (^{\circ}C)$', fontsize=14)
    #plt.ylabel(r'$NEdT$', fontsize=14)
    #plt.xlim(xmin = w1, xmax = w2)
    #txt = plt.figtext(0.7, 0.7, r'$\lambda = %s \mu m$' %(w) +
    #           '\n' + r'$f_{no} = %0.3g$' %(f_25) +
    #            '\n' + r'$ \Delta \sigma = %s \; cm^{-1}$' %(b25) +
    #            '\n' + r'$no. \; bands = %0.3g$' %(((10000/w1) - (10000/w2))/b25), fontsize=10)
    #lgd = plt.legend([ r'$T_{Lens} = %s ^{\circ} C $' %(T_lens_25)],loc='upper left', bbox_to_anchor=(1.02, 1.02), fontsize = 10)
    ##
    #plt.savefig('19-01-28_HyTI_NEdT_CFW.eps', format='eps', dpi=1200, bbox_extra_artists=(lgd,), bbox_inches='tight')
    #plt.show()




#data_SNR = Table([T_c, np.round(SNR_25[0], 3), np.round(SNR_25[0], 3), np.round(SNR_50[0], 3)])
#data_names = ['Temperatures {C}', 'NEdT at lens T: 0C', 'NEdT at lens T: 25C', 'NEdT at lens T: 50C']
#ascii.write(data_SNR, '19-01-28_HyTI_NEdT_CFW.csv', names = data_names, delimiter = ',')
outcoefs.tofile ("C:/Users/harold/workdir/PerfModel/outcoefs")
print "PerfModel Complete"
