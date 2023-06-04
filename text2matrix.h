
void charToArray(unsigned char num, uint8_t line, uint8_t(& arr)[6][8])
{
  int i;
  for (i = 8 * sizeof(num) - 1; i >= 0; i--) {
    uint8_t val = (num & (1 << i)) ? 1 : 0;
    arr[line][7 - i] = val;
  }
}

// A utility function
// to swap the two element
void swap(uint8_t* a, uint8_t* b)
{
  int temp = *a;
  *a = *b;
  *b = temp;
}

// Function to reverse
// the given 2D arr[][]
void reverseArray(uint8_t(& arr)[6][10])
{

  // Traverse each row of arr[][]
  for (int i = 0; i < 6; i++) {

    // Initialise start and end index
    int start = 0;
    int end = 10 - 1;

    // Till start < end, swap the element
    // at start and end index
    while (start < end) {

      // Swap the element
      swap(&arr[i][start],
           &arr[i][end]);

      // Increment start and decrement
      // end for next pair of swapping
      start++;
      end--;
    }
  }
}


void interchange(uint8_t(& arr)[6][10])
{
  int cols = 10;
  int rows = 6;
  int lastRow = rows - 1;

  for (int j = 0; j <= lastRow / 2; j++ ) {
    for (int i = 0; i < cols; i++)
    {
      int t = arr[j][i];
      arr[j][i] = arr[lastRow - j][i];
      arr[lastRow - j][i] = t;
    }
  }
}

void rev2row(uint8_t(& arr)[6][10])
{
  int cols = 10;
  int rows = 6;
  int lastCol = cols - 1;
  for (int j = 0; j < rows; j++ ) {
    j++;
    for (int i = 0; i <= lastCol / 2; i++)
    {
      swap(&arr[j][i],
           &arr[j][lastCol - i]);
    }
  }
}
// convert pixels to strip array
void array2Strip(const uint8_t arr[6][10], uint8_t (&arr2s)[60])
{
  int cols = 10;
  int rows = 6;
  int index = 0;

  for (int j = 0; j < rows; j++ ) {
    for (int i = 0; i < cols; i++)
    {
      arr2s[index] = arr[j][i];
      index++;
    }
  }
}

void text2pixel(std::string text, uint8_t &skip)
{
  uint8_t arr[6][8] {};
  uint8_t conv_arr[6][10] {};
  uint8_t strip_arr[60] {};
  char* str = &text[0];
  byte lastChar = strlen(str) - 1; // Get the position of the last char
  str[lastChar] = '\0';
  const int max_skip_length = lastChar * font4x6[0];

  if (max_skip_length < skip)
    skip = 0;
  // store a pointer to the first character of the string
  char * p = str;

  uint8_t char_num = -1;
  // while the pointed character is not a null terminator
  while (*p)
  {
    // store the character, and increase pointer to the next character
    char c = *p++;
    char_num++;
    uint32_t char_start = (c - font4x6[2]) * font4x6[1] + 3;
    uint32_t char_end = (c - font4x6[2]) * font4x6[1] + 3 + font4x6[1];
    for (int i = char_start; i < char_end; i++)
    {
      charToArray(font4x6[i], i - char_start, arr);
    }

    if (skip > (char_num + 1) * font4x6[0])
      continue;

    for (int a = 0; a < font4x6[1]; a++)
    {
      for (int b = 0; b < font4x6[0]; b++)
      {

        int index = b + font4x6[0] * char_num;

        if (skip > char_num * font4x6[0] && index < 10)
        {
          conv_arr[a][index] = arr[a][b + skip];
        }
        else if (index - skip < 10 && index - skip >= 0)
        {
          conv_arr[a][index - skip] = arr[a][b];
        }

      }
    }
  }

  //reverseArray(display);
  interchange(conv_arr);
  rev2row(conv_arr);
  array2Strip(conv_arr, strip_arr);

  for (int i = 0; i < LED_COUNT; i++) {

    if (strip_arr[i] == 1)setPixel(i, rgb_color[0][0], rgb_color[0][1], rgb_color[0][2]);
    else setPixel(i, 0, 0, 0);
  }
  showStrip();
  delay(100);
  skip++;
}
