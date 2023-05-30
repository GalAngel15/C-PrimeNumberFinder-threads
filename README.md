# C-PrimeNumberFinder-threads

This program is designed to find prime numbers within a specified range. It utilizes multiple threads to enhance performance by parallelizing the computation. The program accepts various command-line arguments to customize its behavior.

## Usage

```shell
./prime_number_finder [OPTIONS]
```

## Command-Line Options

The following options are supported:

- `-l [lower_bound]`: Set the lower bound of the range to search for prime numbers. Defaults to 2 if not provided.
- `-u [upper_bound]`: Set the upper bound of the range to search for prime numbers. Defaults to 100 if not provided.
- `-n [number prints]`: Set the maximum number of prime numbers to print. Defaults to 10 if not provided.
- `-t [threads]`: Set the number of threads to use for computation. Defaults to 4 if not provided.

## Implementation Details

- The program uses the `isprime` function to determine if a given number is prime. It utilizes an internal array of previously found prime numbers to optimize performance.
- Multiple threads are used to divide the computation across different ranges of numbers. Each thread checks a subset of numbers within the specified range.
- A mutex lock (`pthread_mutex_t`) is used to synchronize access to shared resources, such as the prime numbers array and the counter for found primes.

## Building and Running

To compile the program, use the following command:

```shell
gcc -o prime_number_finder prime_number_finder.c -lm -pthread
```

After compiling, you can run the program with appropriate command-line arguments.

**Example:**

```shell
./prime_number_finder -l 10 -u 100 -n 5 -t 2
```

This will print the first 5 prime numbers between 10 and 100 using 2 threads.

## Notes

- For more information on the available command-line options, refer to the `parseargs` function in the code.
- For detailed documentation on the implementation of the prime number finder, refer to the comments within the code.

# מוצא מספרים ראשוניים

התוכנית הזו פותחה כדי למצוא מספרים ראשוניים בתחום מסוים. היא משתמשת בתהליכים מרובים לשיפור ביצועים על ידי מיקבול החישוב. התוכנה מקבלת פרמטרים ממשק הפקודה כדי להתאים אישית את התוכנה לפי הצורך.

## שימוש

```shell
./prime_number_finder [אפשרויות]
```

## אפשרויות ממשק הפקודה

האפשרויות הבאות נתמכות:

- `-l [גבול תחתון]`: מגדיר את הגבול התחתון של הטווח לחיפוש מספרים ראשוניים. ברירת המחדל היא 2 אם לא סופק.
- `-u [גבול עליון]`: מגדיר את הגבול העליון של הטווח לחיפוש מספרים ראשוניים. ברירת המחדל היא 100 אם לא סופק.
- `-n [כמות להדפסה]`: מגביל את כמות המספרים הראשוניים שיש להדפיס. ברירת המחדל היא 10 אם לא סופק.
- `-t [תהליכים]`: מגדיר את מספר התהליכים לשימוש בחישוב. ברירת המחדל היא 4 אם לא סופק.

## פרטים טכניים

-התוכנה משתמשת בפונקציה isprime כדי לקבוע אם מספר נתון הוא מספר ראשוני. היא משתמשת במערך פנימי של מספרים ראשוניים שנמצאו קודם כדי לשפר את הביצועים.
-מספר תהליכים משתמשים בחישוב במקביל על מנת לחלק את המטלה לטווחים שונים של המספרים. כל תהליך בודק תת-קבוצה של מספרים בטווח הנתון.
-נערך נעילת mutex (pthread_mutex_t) לסנכרון גישה למשאבים משותפים, כגון מערך המספרים הראשוניים ומונה המספרים הנמצאים.
