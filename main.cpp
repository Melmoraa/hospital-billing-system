#include <bits/stdc++.h>
using namespace std;

enum class ItemType { MEDICAL, NON_MEDICAL };
string tstr(ItemType t){ return t==ItemType::MEDICAL ? "MEDICAL":"NON_MEDICAL"; }
double round2(double x){ return round(x*100.0)/100.0; }

struct Patient {
    string name; int age; bool insured;
    double coverage;   // 0..1 (e.g., 0.8 = 80% paid by insurer)
    double copay;      // flat amount added if insured
};

struct LineItem {
    string code, desc; double unit; int qty; ItemType type;
    double total() const { return unit * qty; }
};

struct Encounter {
    Patient patient;
    vector<LineItem> items;
};

struct Invoice {
    double medSub{}, nonSub{}, insuranceDisc{}, tax{}, copay{}, grand{};
};

Invoice bill(const Encounter& e, double taxRate) {
    Invoice inv;
    for (auto& li : e.items)
        (li.type==ItemType::MEDICAL ? inv.medSub : inv.nonSub) += li.total();

    inv.medSub  = round2(inv.medSub);
    inv.nonSub  = round2(inv.nonSub);
    inv.copay   = (e.patient.insured ? round2(e.patient.copay) : 0.0);
    inv.insuranceDisc = (e.patient.insured ? round2(inv.medSub * e.patient.coverage) : 0.0);
    double medDue = round2(max(0.0, inv.medSub - inv.insuranceDisc));
    inv.tax   = round2(inv.nonSub * taxRate);
    inv.grand = round2(medDue + inv.nonSub + inv.tax + inv.copay);
    return inv;
}

int gi(const string& q,int lo,int hi){ while(true){ cout<<q; string s; getline(cin,s);
    try{int v=stoi(s); if(v<lo||v>hi) throw 1; return v;}catch(...){cout<<"Try again.\n";}}}
double gd(const string& q,double lo,double hi){ while(true){ cout<<q; string s; getline(cin,s);
    try{double v=stod(s); if(v<lo||v>hi) throw 1; return v;}catch(...){cout<<"Try again.\n";}}}
string gs(const string& q){ cout<<q; string s; getline(cin,s); return s; }
bool yn(const string& q){ while(true){ string s=gs(q+" (y/n): "); if(s=="y"||s=="Y")return true; if(s=="n"||s=="N")return false; }}

int main(){
    ios::sync_with_stdio(false); cin.tie(nullptr);
    const double TAX = 0.075;          // 7.5% on NON_MEDICAL only

    // ---- Input (single encounter for simplicity) ----
    Encounter enc;
    enc.patient.name = gs("Patient name: ");
    enc.patient.age  = gi("Age (0..120): ",0,120);
    enc.patient.insured = yn("Insured?");
    if(enc.patient.insured){
        enc.patient.coverage = gd("Coverage (0..1, e.g., 0.8): ",0,1);
        enc.patient.copay    = gd("Co-pay (>=0): ",0,1e9);
    } else { enc.patient.coverage = 0; enc.patient.copay = 0; }

    int n = gi("How many line items? ",1,100);
    for(int i=0;i<n;i++){
        cout<<"\nItem "<<(i+1)<<":\n";
        LineItem li;
        li.code=gs("  Code (CONSULT/LAB01/...): ");
        li.desc=gs("  Description: ");
        li.unit=gd("  Unit price: ",0,1e9);
        li.qty =gi("  Quantity: ",1,1e6);
        li.type=(gi("  Type (0=MEDICAL,1=NON_MEDICAL): ",0,1)==0?ItemType::MEDICAL:ItemType::NON_MEDICAL);
        enc.items.push_back(li);
    }

    // ---- Billing + Output ----
    Invoice inv = bill(enc, TAX);

    cout << "\n========== INVOICE ==========\n";
    cout << "Patient: " << enc.patient.name << "  Age " << enc.patient.age
         << "  Insured: " << (enc.patient.insured? "Yes":"No") << "\n\n";
    cout << left<<setw(12)<<"CODE"<<setw(18)<<"TYPE"<<setw(28)<<"DESCRIPTION"
         << right<<setw(8)<<"QTY"<<setw(12)<<"UNIT"<<setw(12)<<"TOTAL"<<'\n';
    cout << string(90,'-') << '\n';
    for(auto& li: enc.items){
        cout<<left<<setw(12)<<li.code<<setw(18)<<tstr(li.type)<<setw(28)<<li.desc
            <<right<<setw(8)<<li.qty<<setw(12)<<fixed<<setprecision(2)<<li.unit
            <<setw(12)<<fixed<<setprecision(2)<<li.total()<<'\n';
    }
    cout << string(90,'-') << '\n';
    cout << left<<setw(60)<<"Subtotal (MEDICAL)"     << right<<setw(18)<<fixed<<setprecision(2)<<inv.medSub << '\n';
    cout << left<<setw(60)<<"Insurance Discount"     << right<<setw(18)<<"-"<<fixed<<setprecision(2)<<inv.insuranceDisc << '\n';
    cout << left<<setw(60)<<"Subtotal (NON-MEDICAL)" << right<<setw(18)<<inv.nonSub << '\n';
    cout << left<<setw(60)<<"Tax on NON-MEDICAL"     << right<<setw(18)<<inv.tax << '\n';
    cout << left<<setw(60)<<"Co-pay"                  << right<<setw(18)<<inv.copay << '\n';
    cout << left<<setw(60)<<"GRAND TOTAL (Due)"      << right<<setw(18)<<inv.grand << '\n';
    cout << "=============================\n";
    return 0;
