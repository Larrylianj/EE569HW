%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Siyang Liang
% EE-569-HW2- Problem 2 - Part A - (1)
% Plot the histogram for a given data
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

fp = fopen('/Users/liangsiyang/Documents/USC-learning/EE-569/HW2/images/elaine_LoG_hist.raw','r');
data = fread(fp);
fclose(fp);
image_size = 256*256;
SYMBOL_NUM = 256;
BytesPerPixel = 1;
symbolString = zeros(SYMBOL_NUM,BytesPerPixel);
pdf = zeros(SYMBOL_NUM,BytesPerPixel);
cdf = zeros(SYMBOL_NUM,BytesPerPixel);
color = 1;
for i = 1:length(data)
    for pixel = 1: SYMBOL_NUM
        if data(i) == (pixel-1)
            symbolString(pixel,color) = symbolString(pixel,color) + 1;
        end
    end
    color = color + 1;
    color = rem(color,BytesPerPixel)+1;
end
pdf = symbolString/image_size;
sum = zeros(1,3);
for pixel = 1: SYMBOL_NUM
    for color = 1: BytesPerPixel
        sum(1,color) = sum(1,color) + pdf(pixel,color);
        cdf(pixel,color) = sum(1,color);
    end
end


figure;
stem(0:SYMBOL_NUM-1,symbolString(:,1));
xlabel('symbol');
ylabel('Times');
title('Histogram    Girls  R-channel');
set(gca,'Fontsize',15);
figure;
stem(0:SYMBOL_NUM-1,symbolString(:,2));
xlabel('symbol');
ylabel('Times');
title('Histogram    Girls  G-channel');
set(gca,'Fontsize',15);
figure;
stem(0:SYMBOL_NUM-1,symbolString(:,3));
xlabel('symbol');
ylabel('Times');
title('Histogram    Girls  B-channel');
set(gca,'Fontsize',15);

figure;
stem(0:SYMBOL_NUM-1,cdf(:,1));
xlabel('symbol');
ylabel('cumulative probability');
title('Cdf  Girls  R-channel');
set(gca,'Fontsize',15);
figure;
stem(0:SYMBOL_NUM-1,cdf(:,2));
xlabel('symbol');
ylabel('cumulative probability');
title('Cdf  Girls  G-channel');
set(gca,'Fontsize',15);
figure;
stem(0:SYMBOL_NUM-1,cdf(:,3));
xlabel('symbol');
ylabel('cumulative probability');
title('Cdf  Girls  B-channel');
set(gca,'Fontsize',15);
