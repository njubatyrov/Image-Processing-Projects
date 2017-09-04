
vid = VideoReader('aquarium.avi');

frames1 = cell([1 vid.NumberOfFrames]);

for i = 1 : vid.NumberOfFrames
   img = read(vid,i);
   img = img(:,:,1);
   frames1{i} = img;
end

vidOut = VideoWriter('Result.avi');
open(vidOut);

frames2 = cell([1 vid.NumberOfFrames]);

for i = 1:vid.NumberOfFrames
    frames2{i} = medfilt2(frames1{i}, [3, 3]);
    
    img = img_arr1{i};
    F = fftshift(fft2(img));
    F(1:256, 257) = 0;
    F(258:end, 257) = 0;
   
    ind1 = 300;
    ind2 = 124;
    coef = 30;
    
    for c = 1:10
    
     if ind1 == 257 && ind2 == 257
            ind1 = ind1 - 7;
            ind2 = ind2 + 22;
     else
        F(ind1, ind2) = F(ind1 + coef, ind2 + coef);
            for d = 1:20
                
                F(ind1, ind2 + d) = F(ind1 + coef, ind2 + d + coef);
                F(ind1, ind2 - d) = F(ind1 + coef, ind2 - d + coef);
                F(ind1 + d, ind2) = F(ind1 + d + coef, ind2 + coef);
                F(ind1 - d, ind2) = F(ind1 - d + coef, ind2 + coef);
                F(ind1 + d, ind2 + d) = F(ind1 + d + coef, ind2 + d + coef);
                F(ind1 + d, ind2 - d) = F(ind1 + d + coef, ind2 - d + coef);
                F(ind1 - d, ind2 - d) = F(ind1 - d + coef, ind2 - d + coef);
                F(ind1 - d, ind2 + d) = F(ind1 - d + coef, ind2 + d + coef);
            end
        
        if c == 3
            ind2 = ind2+1;
        end
        
        if c == 4
            ind1 = ind1 - 1;
        end
        
        ind1 = ind1 - 7;
        ind2 = ind2 + 22;
     end
    end
    
    
    for ind1 = 1:size(img, 1)
        for ind2 = 1:size(img, 2)
            if(ind1 == 257 && ind2 == 257)
                continue;
            end
            if(F_norm(ind1, ind2) > 0.5)
                F(ind1, ind2) = 0;% F(ind1 + coef, ind2 + coef);
            end   
        end
    end

    F(1:256, 257) = 0;
    F(258:end, 257) = 0;
    F(:, 246) = F(:, 247);
    F(:, 268) = F(:, 267);
    F(257, 1:256) = F(256, 1:256);
    F(257, 258:end) = F(256, 258:end);

    reference = imread('reference.jpg');
    F_inv = ifft2(ifftshift(F));
    F_inv = abs(double(F_inv));
    F_inv = (F_inv - min(F_inv(:)))/(max(F_inv(:)) - min(F_inv(:)));
    F_inv = imhistmatch(F_inv,ref);
    img = uint8(255*F_inv);
    img = imsharpen(img);
    writeVideo(vidOut, img);
end
   
   close(vidOut);
   