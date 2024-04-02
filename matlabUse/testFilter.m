% 读取 txt 文件中的数据
filename = 'data.txt';  
data = load(filename);

% 将数据分为三列
column1 = data(:, 1);
column2 = data(:, 2);

% 绘制三个图表
subplot(3, 1, 1);  % 创建第一个子图
plot(column1);
title('Data from Column 1');
xlabel('X-axis label');
ylabel('Y-axis label');

subplot(3, 1, 2);  % 创建第二个子图
plot(column2);
title('Data from Column 2');
xlabel('X-axis label');
ylabel('Y-axis label');

Fs = 500;            % Sampling frequency                    
T = 1/Fs;             % Sampling period       
L = 4000;             % Length of signal
t = (0:L-1)*T;        % Time vector


% 绘制FFT结果
figure;
subplot(3, 1, 1);  % 创建第1个子图
Y = fft(column1);
P2 = abs(Y/L);
P1 = P2(1:L/2+1);
P1(2:end-1) = 2*P1(2:end-1);

f = Fs/L*(0:(L/2));

plot(f,P1,"LineWidth",3) 
title("Single-Sided Amplitude Spectrum of S(t)")
xlabel("f (Hz)")
ylabel("|P1(f)|")

subplot(3, 1, 2);  % 创建第2个子图
Y = fft(column2);
P2 = abs(Y/L);
P1 = P2(1:L/2+1);
P1(2:end-1) = 2*P1(2:end-1);

f = Fs/L*(0:(L/2));

plot(f,P1,"LineWidth",3) 
title("Single-Sided Amplitude Spectrum of S(t)")
xlabel("f (Hz)")
ylabel("|P1(f)|")

