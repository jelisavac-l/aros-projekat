# Da u ime ovog fajla polozim osnove finansijskog menadzmenta!

def calculate_compound_interest():
    P = float(input("Enter starting principal: "))
    r = float(input("Enter annual interest rate (e.g., 15 for 15%): "))
    n = int(input("Enter number of compounding periods per year: "))
    y = int(input("Enter the number of years: "))

    FV = P * ((1 + (r / 100.0) / n) ** (n * y))

    print(f"The final amount after {y} years is: {FV:.2f}")

calculate_compound_interest()