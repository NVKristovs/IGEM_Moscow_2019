
# coding: utf-8

# In[32]:


import sys
import os
import traceback
import optparse
import time
import logging


def wavelength_to_rgb(wavelength, gamma=0.8):

    '''This converts a given wavelength of light to an 
    approximate RGB color value. The wavelength must be given
    in nanometers in the range from 380 nm through 750 nm
    (789 THz through 400 THz).
    Based on code by Dan Bruton
    http://www.physics.sfasu.edu/astro/color/spectra.html
    '''

    wavelength = float(wavelength)
    if wavelength >= 380 and wavelength <= 440:
        attenuation = 0.3 + 0.7 * (wavelength - 380) / (440 - 380)
        R = ((-(wavelength - 440) / (440 - 380)) * attenuation) ** gamma
        G = 0.0
        B = (1.0 * attenuation) ** gamma
    elif wavelength >= 440 and wavelength <= 490:
        R = 0.0
        G = ((wavelength - 440) / (490 - 440)) ** gamma
        B = 1.0
    elif wavelength >= 490 and wavelength <= 510:
        R = 0.0
        G = 1.0
        B = (-(wavelength - 510) / (510 - 490)) ** gamma
    elif wavelength >= 510 and wavelength <= 580:
        R = ((wavelength - 510) / (580 - 510)) ** gamma
        G = 1.0
        B = 0.0
    elif wavelength >= 580 and wavelength <= 645:
        R = 1.0
        G = (-(wavelength - 645) / (645 - 580)) ** gamma
        B = 0.0
    elif wavelength >= 645 and wavelength <= 750:
        attenuation = 0.3 + 0.7 * (750 - wavelength) / (750 - 645)
        R = (1.0 * attenuation) ** gamma
        G = 0.0
        B = 0.0
    else:
        R = 0.0
        G = 0.0
        B = 0.0
    return (R, G, B)


# In[81]:


import numpy as np
import matplotlib.pyplot as plt
import shapely.geometry as geom
from shapely.geometry import LineString
from shapely.geometry import Point
from shapely.affinity import rotate

def plot_line(ax, ob,color=None):
    x, y = ob.xy
    ax.plot(x, y, color=color,alpha=0.7, linewidth=1, solid_capstyle='round', zorder=2,)
    
def make_shapely_ark(centerx,centery,radius,start_angle,end_angle,numsegments=1000):
    theta = np.radians(np.linspace(start_angle, end_angle, numsegments))
    x = centerx + radius * np.cos(theta)
    y = centery + radius * np.sin(theta)
    return(geom.LineString(np.column_stack([x, y])))

def getAngle(a, b, c):
    ba = np.array(a) - np.array(b)
    bc = np.array(c) - np.array(b)
    cosine_angle = np.dot(ba, bc) / (np.linalg.norm(ba) * np.linalg.norm(bc))
    angle = np.arccos(cosine_angle)
    return angle

def getExtrapoledLine(p1,p2):
    'Creates a line extrapoled in p1->p2 direction'
    EXTRAPOL_RATIO = 2
    a = p1
    b = (p1[0]+EXTRAPOL_RATIO*(p2[0]-p1[0]), p1[1]+EXTRAPOL_RATIO*(p2[1]-p1[1]) )
    return LineString([a,b])

plt.style.use('seaborn-white')
fig=plt.figure(figsize=(10,10))
ax=fig.add_subplot(111)
ax.set_aspect('equal')


#in mm
rouland_circle_pos=(0,0)
rouland_circle_radius=50

grating_circle_pos=(-rouland_circle_radius,0)
grating_angle=5

entry_slit_angle=200
# in nm
wavelength=500.0
# dvd 740 cd 1600
grating_step=740
diffraction_order=1
entry_slit_angle=180
slit_x=rouland_circle_pos[0]+ rouland_circle_radius * np.cos(np.radians(entry_slit_angle))
slit_y=rouland_circle_pos[1]+ rouland_circle_radius * np.sin(np.radians(entry_slit_angle))

rouland_circle=make_shapely_ark(*rouland_circle_pos,rouland_circle_radius,4*grating_angle,360-4*grating_angle)
plot_line(ax,rouland_circle,color='b')

grating_circle=make_shapely_ark(*grating_circle_pos, 2*rouland_circle_radius,-grating_angle,+grating_angle)
plot_line(ax,grating_circle,color='r')

#adding some unnesessary lines
ax.plot(*np.column_stack((rouland_circle.coords[0], grating_circle.coords[-1])),'b')
ax.plot(*np.column_stack((rouland_circle.coords[-1], grating_circle.coords[0])),'b')


for wavelength in np.arange(400,801,50):
    for diffraction_order in [-1]:
        for start_stop in grating_circle.coords[::200]:
            try:
                l = LineString([(slit_x,slit_y), start_stop])
                plot_line(ax,l,color='k')

                incidence_angle=getAngle((slit_x,slit_y),start_stop,grating_circle_pos)
                diffraction_angle= -np.arcsin(-(diffraction_order*wavelength/grating_step)-np.sin(incidence_angle))


                reflection_line=rotate(l,incidence_angle+diffraction_angle,origin=start_stop,use_radians=True)

                reflection_point=rouland_circle.intersection(getExtrapoledLine(*reflection_line.coords[::-1]))
                full_reflection_line=LineString([start_stop, reflection_point])


                plot_line(ax,full_reflection_line,color=wavelength_to_rgb(wavelength))
            except:
                pass
            
plt.minorticks_on()
plt.grid(b=True, which='both')
plt.show()


# In[44]:


print(plt.style.available)

