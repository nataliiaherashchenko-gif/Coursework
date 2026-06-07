#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <iomanip>
#include <stdexcept>

using namespace std;

class SparePart {
protected:
    string name;
    string number;
    double weightGrams;
    double volumeCm3;

public:
    SparePart(string n, string num, double w, double v) {
        name = n;
        number = num;
        weightGrams = w;
        volumeCm3 = v;
    }
    
    virtual ~SparePart() {}

    double getWeightKg() const { return weightGrams / 1000.0; }
    double getVolumeM3() const { return volumeCm3 / 1000000.0; }
    string getNumber() const { return number; }
    string getName() const { return name; }

    virtual void printInfo() const {
        cout << left << setw(20) << name << " | № " << setw(10) << number 
             << " | Вес: " << setw(6) << getWeightKg() << " кг | Объем: " 
             << setw(6) << getVolumeM3() << " м^3";
    }
};


class Product : public SparePart {
private:
    double cost;
    int productionTimeDays;

public:
    Product(string n, string num, double w, double v, double c, int time)
        : SparePart(n, num, w, v) {
        cost = c;
        productionTimeDays = time;
    }

    double getCost() const { return cost; }
    
    void printInfo() const {
        SparePart::printInfo();
        cout << " | Вартість: $" << setw(6) << cost 
             << " | Виробництво: " << productionTimeDays << " дн.";
    }
};

class Factory {
private:
    vector<Product> catalog;

public:
    void addProductToCatalog(const Product& p) {
        catalog.push_back(p);
    }

    void printFactoryInfo() const {
        cout << "\n--- КАТАЛОГ ЗАВОДУ ---\n";
        if (catalog.empty()) {
            cout << "Каталог порожній.\n";
            return;
        }
        for (size_t i = 0; i < catalog.size(); ++i) {
            catalog[i].printInfo();
            cout << "\n";
        }
        cout << "----------------------\n";
    }

    SparePart* producePart(const string& num) const {
        for (size_t i = 0; i < catalog.size(); ++i) {
            if (catalog[i].getNumber() == num) {
                return new Product(catalog[i]);
            }
        }
        throw invalid_argument("Запчастина з артикулом " + num + " не знайдена в каталозі заводу!");
    }
};

class Warehouse {
private:
    string name;
    string address;
    double dailyRentCost;
    double totalVolumeM3;
    double currentVolumeM3;
    vector<SparePart*> storage;

public:
    Warehouse(string n, string addr, double rent, double vol) {
        name = n;
        address = addr;
        dailyRentCost = rent;
        totalVolumeM3 = vol;
        currentVolumeM3 = 0.0;
    }

    ~Warehouse() {
        for (size_t i = 0; i < storage.size(); ++i) {
            delete storage[i];
        }
    }void printInfo() const {
        cout << "\n=== ІНФОРМАЦІЯ ПРО СКЛАД ===\n";
        cout << "Назва: " << name << " | Адреса: " << address << "\n";
        cout << "Оренда: $" << dailyRentCost << "/доба\n";
        cout << "Обʼєм: " << currentVolumeM3 << " / " << totalVolumeM3 << " м^3\n";
        cout << "Запчастин у наявності: " << storage.size() << "\n";
        
        if (!storage.empty()) {
            cout << "\nВміст складу:\n";
            for (size_t i = 0; i < storage.size(); ++i) {
                storage[i]->printInfo();
                cout << "\n";
            }
        }
        cout << "===========================\n";
    }

    void addPart(SparePart* part) {
        if (part == nullptr) return;

        double partVol = part->getVolumeM3();
        if (currentVolumeM3 + partVol > totalVolumeM3) {
            delete part;
            throw overflow_error("Недостатньо місця на складі! Операцію скасовано.");
        }

        currentVolumeM3 += partVol;
        storage.push_back(part);
        cout << "Запчастина успішно додана на склад.\n";
    }

    void removePart(const string& num) {
        for (auto it = storage.begin(); it != storage.end(); ++it) {
            if ((*it)->getNumber() == num) {
                currentVolumeM3 -= (*it)->getVolumeM3();
                cout << "Запчастина №" << num << " (" << (*it)->getName() << ") успішно видана зі складу.\n";
                
                delete *it;
                storage.erase(it);
                return;
            }
        }
        throw invalid_argument("Запчастина з артикулом " + num + " на складі не знайдена!");
    }
};

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main() {
    setlocale(LC_ALL, "UKRAINIAN");

    Factory factory;
    factory.addProductToCatalog(Product("Двигун V8", "ENG-001", 150000, 2000000, 5000, 14));
    factory.addProductToCatalog(Product("Гальмівні колодки", "BRK-099", 1500, 5000, 45, 1));
    factory.addProductToCatalog(Product("Фара передня", "LGH-102", 2500, 15000, 120, 2));

    Warehouse warehouse("Центральний Хаб", "Промислова 12", 200.0, 5.0);

    int choice = 0;
    while (true) {
        cout << "\n>>> ГОЛОВНЕ МЕНЮ <<<\n";
        cout << "1. Переглянути каталог заводу\n";
        cout << "2. Зробити запчастину та відправити на склад\n";
        cout << "3. Подивитись інформацію про склад\n";
        cout << "4. Забрати запчастину зі складу\n";
        cout << "5. Вихід\n";
        cout << "Ваш вибор: ";
        
        if (!(cin >> choice)) {
            cout << "[ПОМИЛКА] Введіть число!\n";
            clearInput();
            continue;
        }

        try {
            switch (choice) {
                case 1:
                    factory.printFactoryInfo();
                    break;
                case 2: {
                    string art;
                    cout << "Введіть артикул (номер) запчастини з каталога: ";
                    cin >> art;
                
                    SparePart* newPart = factory.producePart(art);
                    warehouse.addPart(newPart);
                    break;
                }
                case 3:
                    warehouse.printInfo();
                    break;
                case 4: {
                    string art;
                    cout << "Введіть артикул запчастини для видачі: ";
                    cin >> art;
                    warehouse.removePart(art);
                    break;
                }
                case 5:
                    cout << "Завершення роботи програми...\n";
                    return 0;
                default:
                    cout << "[ПОМИЛКА] Невірний пункт меню.\n";
            }
        }
        catch (const exception& e){
            cout << "[ЗНАЙДЕНА ПОМИЛКА] " << e.what() << "\n";
        }
    }
    
    return 0;
}
