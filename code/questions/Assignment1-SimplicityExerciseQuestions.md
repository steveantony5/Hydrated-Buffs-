Please include your answers to the questions below with your submission, entering into the space below each question
See [Mastering Markdown](https://guides.github.com/features/mastering-markdown/) for github markdown formatting if desired.

**1. How much current does a single LED draw when the output drive is set to "Strong" with the original code?**

The LED draw only 0.41mA, when the output drive is set to "strong"

**2. After commenting out the standard output drive and uncommenting "Weak" drive, how much current does a single LED draw?**

The LED draw only 0.49mA, when the output drive is set to "weak"

**3. Is there a meaningful difference in current between the answers for question 1 and 2? Please explain your answer, 
referencing the [Mainboard Schematic](https://www.silabs.com/documents/public/schematic-files/WSTK-Main-BRD4001A-A01-schematic.pdf) and [AEM Accuracy](https://www.silabs.com/documents/login/user-guides/ug279-brd4104a-user-guide.pdf) section of the user's guide where appropriate.**

No, there in no much difference between the first two case. This is due to the fact that the LED circuitary consumes 
less than 0.5mA current (according to the datasheet), irrespective of the state (strong or weak). Now subtracting the
current loss due to the pull-down resistor, gives the actual current consumed by a LED which is around 0.4mA

**4. Using the Energy Profiler with "weak" drive LEDs, what is the average current and energy measured with only LED1 turning on in the main loop?**

Average Current = 4.89 mA and Energy measured = 11.94 μWh

**5. Using the Energy Profiler with "weak" drive LEDs, what is the average current and energy measured with both LED1 and LED0 turning on in the main loop?**

Average Current = 5.05 mA and Energy measured = 12.12 μWh
