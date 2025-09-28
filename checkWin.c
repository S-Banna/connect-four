Boolean checkWin (Char color, int row, int column, Char [][]arr){
    int left= column-1;
    int right= column+1;
    int down= row-1;
    int count;
    if(column<=3){
        if(row<=2){
            if(column==3){
                if(arr[down][left]==color){
                count=2;
                for(int i=1; i<3; i++){
                    if(arr[down-i][right-i]==color) count++;
                    else break;
                }
                if(count==4) return true;
            }
            }
            if(arr[row][right]==color){
                count=2;
                for(int i=1; i<3; i++){
                    if(arr[row][right+i]==color) count++;
                    else break;
                }
                if(count==4) return true;
            }
            if(arr[down][right]==color){
                count=2;
                for(int i=1; i<3; i++){
                    if(arr[down-i][right+i]==color) count++;
                    else break;
                }
                if(count==4) return true;
            }
            if(arr[down][column]==color){
                count=2;
                for(int i=1; i<3; i++){
                    if(arr[down-i][column]==color) count++;
                    else break;
                }
                if(count==4) return true;
            }
        }
         else{
            if(arr[row][right]==color){
                count=2;
                for(int i=1; i<3; i++){
                    if(arr[row][right+i]==color) count++;
                    else break;
                }
                if(count==4) return true;
            }
            }
    }
    
    return false;
}