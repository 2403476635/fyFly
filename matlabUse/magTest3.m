clear all;
clc;
filename = 'data_mag.txt';  
data = load(filename);
[a,b] = ellipsoidFitting(data)
