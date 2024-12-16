#include <cairomm/context.h>
#include <gtkmm.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <iomanip>   // para usar put_time na parte de salvar o historico

using namespace std;

class data{  // classe abstrata
    public:

    virtual void add(const string &copy){

    }

    virtual void remove(const string &item){

    }

    virtual void save(const string& filename){

    }

    virtual ~data() = default;
};

class manipulates_history: public data{
    private: 

    vector <string> history;


    public:

    void add(const string &copy) override{
        history.push_back(copy);
    }

    void remove(const string &item) override{
        auto it = find(history.begin(), history.end(), item);

        if(it != history.end()){ 
            history.erase(it);
            cout << "Removido do historico: " << item << endl;
        } else {
            cout << "Item nao encontrado no historico." << endl;
        }
    }
    
    void save(const string& filename) override{
        ofstream file(filename);                    // escrita no arquivo

        if(!file){
            cout << "Erro ao abrir o arquivo para escrita." << endl;
            return;
        }
 
        for(auto &entry : history){ 
            file << entry << endl; 
        }

        file.close();
        cout << "1) Historico salvo em: " << filename << endl;
    } 
};

class MainApp : public Gtk::Window {
   public:
    MainApp() : clipboard_content("") {
        set_app_paintable(true);
        auto screen1 = get_screen();
        auto visual = screen1->get_rgba_visual();
        if (visual) {
            gtk_widget_set_visual(Gtk::Widget::gobj(), visual->gobj());
        } else {
            g_warning("RGBA not found");
        }

        auto css_provider = Gtk::CssProvider::create();
        css_provider->load_from_data(R"(
            .list-row {
                background-color: lightgray;
                border-radius: 10px;
                color: black;
                padding: 10px;
            }
            .box-container {
                border-radius: 20px;
                margin: 5px;
                padding: 5px;
            }
            .app-container {
                background-color: #2a2a2a;
                border-radius: 20px;
                padding: 10px;
            }
        )");
        auto screen = Gdk::Display::get_default()->get_default_screen();
        Gtk::StyleContext::add_provider_for_screen(
            screen, css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);

        set_default_size(300, 400);
        set_decorated(false);
        set_position(Gtk::WIN_POS_CENTER);

        app_container.set_orientation(Gtk::ORIENTATION_VERTICAL);
        app_container.get_style_context()->add_class("app-container");

        /////////////////////////// adicionar //////////////////////////////
        input_field.set_placeholder_text("Digite o que quer adicionar");
        input_field.set_max_length(10);
        input_field.set_hexpand(true);

        add_button.set_label("Adicionar");
        add_button.signal_clicked().connect(
            sigc::mem_fun(*this, &MainApp::on_add_button_clicked));
        //////////////////////////////////////////////////////////////////

        ////////////////////////////// remoção ////////////////////////// 
        remove_input_field.set_placeholder_text("Digite o que quer remover");
        remove_input_field.set_max_length(10);
        remove_input_field.set_hexpand(true);

        remove_button.set_label("Remover");
        remove_button.signal_clicked().connect(
            sigc::mem_fun(*this, &MainApp::on_remove_button_clicked));
        //////////////////////////////////////////////////////////////////////

        /////////////////////////// salvamento //////////////////////////////////
        save_button.set_label("Salvar Historico");
        save_button.signal_clicked().connect(
            sigc::mem_fun(*this, &MainApp::on_save_button_clicked));

        app_container.pack_start(save_button, Gtk::PACK_SHRINK);
        /////////////////////////////////////////////////////////////////////////

        ///////////////////////////////////// adicionar //////////////////////////
        auto input_box = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL);
        input_box->pack_start(input_field, true, true);
        input_box->pack_start(add_button, false, false);
        app_container.pack_start(*input_box, Gtk::PACK_SHRINK);
        ///////////////////////////////////////////////////////////////////////

        ////////////////////////////////remoção /////////////////////////////////
        auto remove_box = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL);
        remove_box->pack_start(remove_input_field, true, true);
        remove_box->pack_start(remove_button, false, false);
        app_container.pack_start(*remove_box, Gtk::PACK_SHRINK);
        /////////////////////////////////////////////////////////////////////////

        /////////////////////////// mover a interface //////////////////////////
        signal_button_press_event().connect(
            sigc::mem_fun(*this, &MainApp::on_button_press));
        signal_motion_notify_event().connect(
            sigc::mem_fun(*this, &MainApp::on_motion_notify));
        ///////////////////////////////////////////////////////////////////////
        
        scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC,
                                   Gtk::POLICY_AUTOMATIC);
        scrolled_window.add(box);

        box.set_orientation(Gtk::ORIENTATION_VERTICAL);
        box.set_spacing(10);

        box.get_style_context()->add_class("box-container");

        for (int i = 0; i < 20; ++i) {
            auto event_box =
                Gtk::make_managed<Gtk::EventBox>();

            event_box->set_size_request(200, 100);
            event_box->get_style_context()->add_class("list-row");

            auto label = Gtk::make_managed<Gtk::Label>(std::to_string(i + 1));
            event_box->add(*label);

            event_box->signal_button_press_event().connect(
                sigc::bind<Gtk::Label*>(sigc::mem_fun(*this, &MainApp::on_copy_selected), label));

            box.pack_start(*event_box, Gtk::PACK_SHRINK);

            history.add(to_string(i + 1));
        }
        app_container.pack_start(scrolled_window, Gtk::PACK_EXPAND_WIDGET);
        add(app_container);

        signal_key_press_event().connect(
            sigc::mem_fun(*this, &MainApp::on_key_press), false);
        
        show_all_children();
    }

    ~MainApp(){
        history.save("history.txt");  
    }

    string clipboard_content;

    void add_copy(const string &nome, bool n){
        clipboard_content = nome;

        if(n){
            cout << "selecionado para colar: " << clipboard_content << endl;
        }
        
        auto event_box =
            Gtk::make_managed<Gtk::EventBox>();

        event_box->set_size_request(200, 100);
        event_box->get_style_context()->add_class("list-row");

        auto label = Gtk::make_managed<Gtk::Label>(clipboard_content);
        event_box->add(*label);

        event_box->signal_button_press_event().connect(
            sigc::bind<Gtk::Label*>(sigc::mem_fun(*this, &MainApp::on_copy_selected), label));

        box.pack_start(*event_box, Gtk::PACK_SHRINK);
        show_all_children();

        history.add(clipboard_content);          
    }
    
    void save_copies(){
        history.save("history.txt");
    }

   protected:
    Gtk::ScrolledWindow scrolled_window;
    Gtk::Box box;
    Gtk::Box app_container;

    Gtk::Entry input_field;
    Gtk::Button add_button;

    Gtk::Entry remove_input_field;  // para digitar o que quer remover
    Gtk::Button remove_button;  // botao para remover

    Gtk::Label *selected_label = nullptr;

    Gtk::Button save_button;

    manipulates_history history;
    string last_copied;

    int drag_x = 0, drag_y = 0;

    bool is_copied = false;  // ver se ja deu ctrl c ou nao, naquilo que esta querendo colar
    int flag = 0;
    int flag2 = 0;

    bool on_copy_selected(GdkEventButton* event, Gtk::Label *label){
        clipboard_content = label->get_text();
        selected_label = label;
        cout << "selecionado: " << clipboard_content << endl;
        flag = 0;

        return false;
    }

    bool on_key_press(GdkEventKey *key_event){ 
        string text;

        if((key_event->state & GDK_CONTROL_MASK) && key_event->keyval == GDK_KEY_c){
            is_copied = true;
            if(is_copied && !clipboard_content.empty()){
                cout << "copiado: " << clipboard_content << endl;
                flag = 1;
                flag2 = 1;
            } else {
                cout << "Nada para copiar." << endl;
            }

            return true;
        } else if((key_event->state & GDK_CONTROL_MASK) && key_event->keyval == GDK_KEY_v){
            if(flag == 1 && !clipboard_content.empty()){
                last_copied = paste_copy();
            } else if(flag2 == 1 && !clipboard_content.empty()){
                add_copy(last_copied, false);
                cout << "colado: " << last_copied << endl;
            } else {
                cout << "Nada para colar." << endl;
            }

            return true;
        } else if(key_event->keyval == GDK_KEY_Delete || key_event->keyval == GDK_KEY_BackSpace){
            if(selected_label){
                text = selected_label->get_text();
                remove_input_field.set_text(text);
                on_remove_button_clicked();
                selected_label = nullptr;
            } else if(!remove_input_field.get_text().empty()){
                on_remove_button_clicked();
            } else {
                cout << "Nada para remover." << endl;
            }

            return true;
        }

        return false;
    }

    string paste_copy(){
        if(!clipboard_content.empty()){
            auto event_box =
                Gtk::make_managed<Gtk::EventBox>();

            event_box->set_size_request(200, 100);
            event_box->get_style_context()->add_class("list-row");

            auto label = Gtk::make_managed<Gtk::Label>(clipboard_content);
            event_box->add(*label);

            event_box->signal_button_press_event().connect(
                sigc::bind<Gtk::Label*>(sigc::mem_fun(*this, &MainApp::on_copy_selected), label));

            box.pack_start(*event_box, Gtk::PACK_SHRINK);
            show_all_children();

            history.add(clipboard_content);

            cout << "colado: " << clipboard_content << endl;           
        }

        return clipboard_content;
    }

    void on_add_button_clicked(){
        string text = input_field.get_text();

        if (!text.empty()) {
            add_copy(text, true);
            input_field.set_text(""); 
        } else {
            cout << "Nada para adicionar pela interface." << endl;
        }
    }

    void on_remove_button_clicked(){
        string text = remove_input_field.get_text();

        if(!text.empty()){
            auto children = box.get_children();
            for(auto *child : children){
                auto *event_box = dynamic_cast<Gtk::EventBox*>(child);

                if(event_box){
                    auto *label = dynamic_cast<Gtk::Label*>(event_box->get_child());

                    if(label && label->get_text() == text){
                        box.remove(*event_box);
                        history.remove(text);
                        show_all_children();
                        cout << "Removido: " << text << endl;
                        remove_input_field.set_text("");
                        return;
                    }
                }
            }

            remove_input_field.set_text("");
            cout << "Texto nao encontrado para remover." << endl;
        } else {
            cout << "Nada para remover pela interface" << endl;
        }
    }

    bool on_button_press(GdkEventButton *event){  // armazena a posição do click inicial
        if(event->type == GDK_BUTTON_PRESS && event->button == 1){    // click esquerdo
            drag_x = static_cast<int>(event->x_root);  
            drag_y = static_cast<int>(event->y_root);
            return true;
        }

        return false;
    }

    int x, y, delta_x, delta_y, window_x, window_y;

    bool on_motion_notify(GdkEventMotion *event){  // move a janela
        if((event->state & GDK_BUTTON1_MASK)){
            x = static_cast<int>(event->x_root);
            y = static_cast<int>(event->y_root);

            delta_x = x - drag_x;
            delta_y = y - drag_y;

            get_position(window_x, window_y);

            move(window_x + delta_x, window_y + delta_y);

            drag_x = x;
            drag_y = y;

            return true;
        }

        return false;
    }

    void on_save_button_clicked(){
        ostringstream oss;    // // escrita no arquivo
        string filename;
        auto t = time(nullptr);
        auto tm = *localtime(&t);
       
        oss << "historico_" << put_time(&tm, "%d-%m-%Y_%H-%M-%S") << ".txt";
        
        filename = oss.str();
        history.save(filename);
        cout << "2) Historico salvo em: " << filename << endl;
    }
};
    
int main(int argc, char* argv[]) {
    auto app = Gtk::Application::create(argc, argv); 
    MainApp janela;

    janela.add_copy("teste", true);

    return app->run(janela);
}