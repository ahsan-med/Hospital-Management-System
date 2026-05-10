#define _CRT_SECURE_NO_WARNINGS
#include "GUI.h"
#include "FileHandler.h"
#include "Storage.h"
#include "Patient.h"
#include "Doctor.h"
#include "Admin.h"
#include "Appointment.h"
#include "Bill.h"
#include "Prescription.h"
#include "Validator.h"
#include "HospitalException.h"
#include <ctime>
#include <sstream>
using namespace std;

// Helper functions
static string intToStr(int n) { ostringstream s; s << n; return s.str(); }
static string floatToStr(float f) { ostringstream s; s << f; return s.str(); }
static float strToFloat(const string& s) { return s.empty() ? 0.f : stof(s); }
static int strToInt(const string& s) { return s.empty() ? 0 : stoi(s); }

static string getTodayStr() {
    time_t t = time(nullptr); char buf[16];
    strftime(buf, sizeof(buf), "%d-%m-%Y", localtime(&t));
    return string(buf);
}

//correct mouse mapping for fullscreen window 
sf::Vector2f getMappedMousePos(const sf::RenderWindow& win) {
    sf::Vector2i pixel = sf::Mouse::getPosition(win);
    return win.mapPixelToCoords(pixel);
}

//drawing helpers
static void drawCard(sf::RenderWindow& win, float x, float y, float w, float h, sf::Color fill, sf::Color outline, float th = 1.f) { 
    sf::RectangleShape r(sf::Vector2f(w, h));
    r.setPosition(x, y);
    r.setFillColor(fill);
    r.setOutlineColor(outline);
    r.setOutlineThickness(th);
    win.draw(r);
}

//reusable setup helpers
static void initTitle(sf::Text& t, AssetManager& a, const string& s, float x, float y, int size = 28) {
    t.setFont(a.getFont(true));
    t.setString(s);
    t.setCharacterSize(size);
    t.setFillColor(sf::Color::White);
    t.setPosition(x, y);
}
static float centeredY(float cardH) {
    return 70.f + (1010.f - cardH) / 2.f;
}
static void drawCenteredCard(sf::RenderWindow& win, float cardW, float cardH) {
    float x = (1920.f - cardW) / 2.f;
    float y = centeredY(cardH);
    drawCard(win, x, y, cardW, cardH,
        sf::Color(18, 26, 46), sf::Color(30, 100, 200), 2.f);
}
static void drawTopBar(sf::RenderWindow& win, GUIContext& ctx, const string& right) {
    sf::RectangleShape bar(sf::Vector2f(1920.f, 70.f));
    bar.setFillColor(Theme::HEADER_BG);
    win.draw(bar);

    sf::Sprite logo;
    logo.setTexture(ctx.assets->getTexture(Assets::LOGO));
    logo.setPosition(18.f, 8.f);
    float s = 54.f / logo.getLocalBounds().height;
    logo.setScale(s, s);
    win.draw(logo);

    sf::Text title;
    title.setFont(ctx.assets->getFont(true));
    title.setString("    MEDICORE");
    title.setCharacterSize(28);
    title.setFillColor(sf::Color::White);
    title.setPosition(82.f, 18.f);
    win.draw(title);

    sf::Text rt;
    rt.setFont(ctx.assets->getFont(false));
    rt.setString(right);
    rt.setCharacterSize(18);
    rt.setFillColor(sf::Color(100, 160, 230));
    rt.setPosition(1920.f - rt.getLocalBounds().width - 24.f, 22.f);
    win.draw(rt);
}

// Asset Manager
AssetManager::AssetManager() :m_count(0) {
    m_fontRegular.loadFromFile(Assets::FONT_REGULAR);
    m_fontBold.loadFromFile(Assets::FONT_BOLD);
}
int AssetManager::findKey(const string& p)const {
    for (int i = 0; i < m_count; i++) if (m_keys[i] == p) return i; return -1;
}
sf::Texture& AssetManager::getTexture(const string& path) {
    int idx = findKey(path);
    if (idx >= 0) return m_textures[idx];
    m_keys[m_count] = path;
    if (!m_textures[m_count].loadFromFile(path)) {
        sf::Image img;
        img.create(64, 64, sf::Color(20, 30, 50));
        m_textures[m_count].loadFromImage(img);
    }
    return m_textures[m_count++];
}
sf::Font& AssetManager::getFont(bool bold) {
    return bold ? m_fontBold : m_fontRegular;
}

// UI Button, uses passed mousePos instead of raw pixel
UIButton::UIButton() :m_iconTex(nullptr), m_hasIcon(false),
m_hovered(false), m_clicked(false), m_enabled(true), m_primary(true) {
}

void UIButton::init(float x, float y, float w, float h, const string& label, AssetManager& assets, bool primary) {
    m_primary = primary;
    m_hasIcon = false;
    m_rect.setSize(sf::Vector2f(w, h));
    m_rect.setPosition(x, y);
    m_rect.setOutlineThickness(0.f);
    m_text.setFont(assets.getFont(true));
    m_text.setString(label);
    m_text.setCharacterSize(18);
    updateVisual();
}
void UIButton::initWithIcon(float x, float y, float w, float h, const string& label, const string& iconPath, AssetManager& assets) {
    init(x, y, w, h, label, assets, true);
    m_hasIcon = true;
    m_iconTex = &assets.getTexture(iconPath);
    m_icon.setTexture(*m_iconTex);
    float s = (h - 10.f) / m_icon.getLocalBounds().height;
    m_icon.setScale(s, s);
    m_icon.setPosition(x + 8.f, y + 5.f);
}
void UIButton::updateVisual() {
    if (!m_enabled) m_rect.setFillColor(sf::Color(40, 50, 65));
    else if (m_hovered)
        m_rect.setFillColor(m_primary ? Theme::BTN_HOVER : sf::Color(20, 35, 65));
    else
        m_rect.setFillColor(m_primary ? Theme::BTN_PRIMARY : sf::Color(16, 26, 46));
    m_text.setFillColor(m_primary ? Theme::BTN_TEXT : Theme::TEXT_DARK);
    sf::FloatRect tb = m_text.getLocalBounds();
    sf::FloatRect rb = m_rect.getGlobalBounds();
    float tx = rb.left + (rb.width - tb.width) / 2.f - tb.left;
    float ty = rb.top + (rb.height - tb.height) / 2.f - tb.top;
    if (m_hasIcon) tx = rb.left + 36.f;
    m_text.setPosition(tx, ty);
}
void UIButton::handleEvent(const sf::Event& e, const sf::RenderWindow& /*win*/, sf::Vector2f mp) {
    if (!m_enabled) return;
    sf::FloatRect b = m_rect.getGlobalBounds();
    bool over = b.contains(mp);
    m_hovered = over;
    if (e.type == sf::Event::MouseButtonReleased &&
        e.mouseButton.button == sf::Mouse::Left && over)
        m_clicked = true;
    updateVisual();
}
void UIButton::draw(sf::RenderWindow& win) {
    win.draw(m_rect);
    if (m_hasIcon) win.draw(m_icon);
    win.draw(m_text);
}
bool UIButton::isClicked() const { return m_clicked; }
void UIButton::resetClick() { m_clicked = false; }
void UIButton::setEnabled(bool e) { m_enabled = e; updateVisual(); }
void UIButton::setLabel(const string& l) { m_text.setString(l); updateVisual(); }
void UIButton::setPosition(float x, float y) { m_rect.setPosition(x, y); updateVisual(); }

// UI Input Box, uses passed mousePos
UIInputBox::UIInputBox() :m_focused(false), m_isPassword(false), m_readOnly(false) {}
void UIInputBox::init(float x, float y, float w, float h, const string& placeholder, AssetManager& assets, bool isPassword) {
    m_isPassword = isPassword; m_buffer.clear();
    m_rect.setSize(sf::Vector2f(w, h)); m_rect.setPosition(x, y);
    m_rect.setFillColor(Theme::INPUT_BG);
    m_rect.setOutlineThickness(2.f);
    m_rect.setOutlineColor(Theme::INPUT_BORDER);
    m_text.setFont(assets.getFont(false));
    m_text.setCharacterSize(17);
    m_text.setFillColor(Theme::TEXT_DARK);
    m_text.setPosition(x + 10.f, y + (h - 18.f) / 2.f);
    m_placeholder.setFont(assets.getFont(false));
    m_placeholder.setCharacterSize(17);
    m_placeholder.setFillColor(Theme::TEXT_GRAY);
    m_placeholder.setString(placeholder);
    m_placeholder.setPosition(x + 10.f, y + (h - 18.f) / 2.f);
}
void UIInputBox::handleEvent(const sf::Event& e, const sf::RenderWindow& /*win*/, sf::Vector2f mp) {
    if (e.type == sf::Event::MouseButtonPressed) {
        m_focused = m_rect.getGlobalBounds().contains(mp);
        m_rect.setOutlineColor(m_focused ? Theme::INPUT_FOCUS : Theme::INPUT_BORDER);
    }
    if (!m_focused) return;
    if (e.type == sf::Event::TextEntered) {
        if (m_readOnly) return;
        if (e.text.unicode == 8) { if (!m_buffer.empty()) m_buffer.pop_back(); }
        else if (e.text.unicode >= 32 && e.text.unicode < 127)
            m_buffer += (char)e.text.unicode;
        updateText();
    }
}
void UIInputBox::updateText() {
    if (m_isPassword) m_text.setString(string(m_buffer.size(), '*'));
    else             m_text.setString(m_buffer);
}
void UIInputBox::draw(sf::RenderWindow& win) {
    win.draw(m_rect);
    if (m_buffer.empty() && !m_focused) win.draw(m_placeholder);
    else win.draw(m_text);
    if (m_focused && (int)(m_cursorClock.getElapsedTime().asSeconds() * 2) % 2 == 0) {
        float cx = m_text.getPosition().x + m_text.getLocalBounds().width + 2.f;
        float cy = m_text.getPosition().y;
        sf::RectangleShape cur(sf::Vector2f(2.f, 18.f));
        cur.setPosition(cx, cy);
        cur.setFillColor(sf::Color(100, 180, 255));
        win.draw(cur);
    }
}
string UIInputBox::getText() const { return m_buffer; }
void UIInputBox::clear() { m_buffer.clear(); updateText(); }
void UIInputBox::setFocused(bool f) {
    m_focused = f;
    m_rect.setOutlineColor(f ? Theme::INPUT_FOCUS : Theme::INPUT_BORDER);
}
bool UIInputBox::isFocused()  const { return m_focused; }
void UIInputBox::setReadOnly(bool r) { m_readOnly = r; }

// UI Tile, uses mousePos
UITile::UITile() :m_iconTex(nullptr), m_hovered(false), m_clicked(false) {}

void UITile::init(float x, float y, float tileSize, const string& label, const string& iconPath, AssetManager& assets) {
    m_shadow.setSize(sf::Vector2f(tileSize, tileSize));
    m_shadow.setPosition(x + 4.f, y + 5.f);
    m_shadow.setFillColor(sf::Color(0, 20, 60, 100));
    m_card.setSize(sf::Vector2f(tileSize, tileSize));
    m_card.setPosition(x, y);
    m_card.setFillColor(Theme::TILE_BG);
    m_card.setOutlineThickness(1.5f);
    m_card.setOutlineColor(Theme::TILE_BORDER);
    m_iconTex = &assets.getTexture(iconPath);
    m_icon.setTexture(*m_iconTex);
    float iconSize = tileSize * 0.46f;         // larger icon
    float maxDim = m_icon.getLocalBounds().width > m_icon.getLocalBounds().height
                 ? m_icon.getLocalBounds().width : m_icon.getLocalBounds().height;
    float s = iconSize / maxDim;
    m_icon.setScale(s, s);
    float actualW = m_icon.getLocalBounds().width * s;
    float actualH = m_icon.getLocalBounds().height * s;
    m_icon.setPosition(x + (tileSize - actualW) / 2.f, y + tileSize * 0.14f + (iconSize - actualH) / 2.f);
    m_label.setFont(assets.getFont(true));  // bold
    m_label.setString(label);
    m_label.setCharacterSize(16);           // larger text
    m_label.setFillColor(Theme::TEXT_DARK);
    sf::FloatRect lb = m_label.getLocalBounds();
    m_label.setPosition(x + (tileSize - lb.width) / 2.f - lb.left, y + tileSize * 0.70f);
}
void UITile::handleEvent(const sf::Event& e, const sf::RenderWindow& /*win*/, sf::Vector2f mp) {
    bool over = m_card.getGlobalBounds().contains(mp);
    m_hovered = over;
    m_card.setFillColor(over ? Theme::TILE_HOVER : Theme::TILE_BG);
    if (e.type == sf::Event::MouseButtonReleased &&
        e.mouseButton.button == sf::Mouse::Left && over)
        m_clicked = true;
}
void UITile::draw(sf::RenderWindow& win) {
    win.draw(m_shadow);
    win.draw(m_card);
    win.draw(m_icon);
    win.draw(m_label);
}
bool UITile::isClicked() const { return m_clicked; }
void UITile::resetClick() { m_clicked = false; }

// UI Table, row hover/select, uses mousePos
UITable::UITable() :m_cols(0), m_rowCount(0), m_selectedRow(-1),
m_hoveredRow(-1), m_scrollOffset(0.f),
m_x(0), m_y(0), m_w(0), m_h(0), m_font(nullptr) {
}
void UITable::init(float x, float y, float w, float h, const string* headers, int colCount, AssetManager& assets) {
    m_x = x; m_y = y; m_w = w; m_h = h; m_cols = colCount;
    m_font = &assets.getFont(false);
    m_rowCount = 0; m_scrollOffset = 0.f;
    m_selectedRow = -1; m_hoveredRow = -1;
    float cw = w / colCount;
    for (int i = 0; i < colCount; i++) { m_colW[i] = cw; m_headers[i] = headers[i]; }
    m_bg.setSize(sf::Vector2f(w, h));
    m_bg.setPosition(x, y);
    m_bg.setFillColor(sf::Color(12, 16, 28));
    m_bg.setOutlineThickness(1.f);
    m_bg.setOutlineColor(Theme::DIVIDER);
    m_headerBar.setSize(sf::Vector2f(w, 36.f));
    m_headerBar.setPosition(x, y);
    m_headerBar.setFillColor(Theme::HEADER_BG);
}
void UITable::addRow(const string* cells) {
    if (m_rowCount >= MAX_ROWS) return;
    for (int c = 0; c < m_cols; c++) m_data[m_rowCount][c] = cells[c];
    m_rowCount++;
}
void UITable::clearRows() { m_rowCount = 0; m_scrollOffset = 0.f; m_selectedRow = -1; m_hoveredRow = -1; }
string UITable::getSelectedCell(int col) const {
    if (m_selectedRow < 0 || m_selectedRow >= m_rowCount || col < 0 || col >= m_cols)
        return "";
    return m_data[m_selectedRow][col];
}
//detect row clicks with mapped position
void UITable::handleEvent(const sf::Event& e, sf::Vector2f mp) {
    if (e.type == sf::Event::MouseWheelScrolled)
        m_scrollOffset -= e.mouseWheelScroll.delta * 24.f;
    if (m_scrollOffset < 0.f) m_scrollOffset = 0.f;
    float maxScroll = m_rowCount * 30.f - (m_h - 36.f);
    if (maxScroll > 0.f && m_scrollOffset > maxScroll) m_scrollOffset = maxScroll;

    float rowH = 30.f;
    float startY = m_y + 36.f - m_scrollOffset;
    m_hoveredRow = -1;
    for (int r = 0; r < m_rowCount; r++) {
        float ry = startY + r * rowH;
        sf::FloatRect rowRect(m_x, ry, m_w, rowH);
        if (rowRect.contains(mp)) {
            m_hoveredRow = r;
            if (e.type == sf::Event::MouseButtonReleased &&
                e.mouseButton.button == sf::Mouse::Left)
                m_selectedRow = r;
        }
    }
}
void UITable::draw(sf::RenderWindow& win) {
    win.draw(m_bg);
    win.draw(m_headerBar);
    sf::Text hdr;
    hdr.setFont(*m_font);
    hdr.setCharacterSize(15);
    hdr.setFillColor(sf::Color::White);
    float cx = m_x + 8.f;
    for (int c = 0; c < m_cols; c++) {
        hdr.setString(m_headers[c]);
        hdr.setPosition(cx, m_y + 8.f);
        win.draw(hdr);
        cx += m_colW[c];
    }
    sf::Text cell;
    cell.setFont(*m_font);
    cell.setCharacterSize(14);
    float rowH = 30.f;
    float startY = m_y + 36.f - m_scrollOffset;
    sf::RectangleShape div(sf::Vector2f(m_w, 1.f));
    div.setFillColor(Theme::DIVIDER);
    for (int r = 0; r < m_rowCount; r++) {
        float ry = startY + r * rowH;
        if (ry + rowH<m_y + 36.f || ry>m_y + m_h) continue;
        sf::RectangleShape row(sf::Vector2f(m_w - 2.f, rowH - 1.f));
        row.setPosition(m_x + 1.f, ry);
        // selection > hover > alternating
        if (r == m_selectedRow)     row.setFillColor(Theme::ROW_SELECT);
        else if (r == m_hoveredRow) row.setFillColor(Theme::ROW_HOVER);
        else row.setFillColor(r % 2 == 0 ? sf::Color(12, 16, 28) : sf::Color(14, 22, 38));
        win.draw(row);
        float rcx = m_x + 8.f;
        cell.setFillColor(Theme::TEXT_DARK);
        for (int c = 0; c < m_cols; c++) {
            cell.setString(m_data[r][c]);
            cell.setPosition(rcx, ry + 6.f);
            win.draw(cell);
            rcx += m_colW[c];
        }
        div.setPosition(m_x, ry + rowH - 1.f);
        win.draw(div);
    }
}

// UI Message Box
UIMessageBox::UIMessageBox()
    :m_visible(false), m_ok(false), m_cancel(false), m_hasCancel(false) {
}
void UIMessageBox::init(AssetManager& assets) {
    m_overlay.setSize(sf::Vector2f(1920.f, 1080.f));
    m_overlay.setFillColor(sf::Color(0, 0, 0, 140));
    m_panel.setSize(sf::Vector2f(560.f, 260.f));
    m_panel.setPosition(680.f, 410.f);
    m_panel.setFillColor(sf::Color(16, 22, 36));
    m_panel.setOutlineThickness(2.f);
    m_panel.setOutlineColor(Theme::TILE_BORDER);
    m_title.setFont(assets.getFont(true));
    m_title.setCharacterSize(22);
    m_title.setFillColor(sf::Color::White);
    m_title.setPosition(700.f, 422.f);
    m_body.setFont(assets.getFont(false));
    m_body.setCharacterSize(17);
    m_body.setFillColor(Theme::TEXT_DARK);
    m_body.setPosition(695.f, 464.f);
    m_btnOK.init(1050.f, 620.f, 110.f, 42.f, "OK", assets, true);
    m_btnCancel.init(920.f, 620.f, 110.f, 42.f, "Cancel", assets, false);
    m_visible = false;
    m_ok = false;
    m_cancel = false;
}
void UIMessageBox::show(const string& t, const string& b, bool hasCancel) {
    m_title.setString(t);
    m_body.setString(b);
    m_hasCancel = hasCancel;
    m_visible = true;
    m_ok = m_cancel = false;
}
bool UIMessageBox::isVisible() const { return m_visible; }
void UIMessageBox::handleEvent(const sf::Event& e, const sf::RenderWindow& win, sf::Vector2f mp) {
    if (!m_visible) return;
    m_btnOK.handleEvent(e, win, mp);
    if (m_hasCancel) m_btnCancel.handleEvent(e, win, mp);
    if (m_btnOK.isClicked()) { m_ok = true; m_visible = false; m_btnOK.resetClick(); }
    if (m_hasCancel && m_btnCancel.isClicked()) {
        m_cancel = true; m_visible = false; m_btnCancel.resetClick();
    }
}
void UIMessageBox::draw(sf::RenderWindow& win) {
    if (!m_visible) return;
    win.draw(m_overlay);
    win.draw(m_panel);
    win.draw(m_title);
    win.draw(m_body);
    m_btnOK.draw(win);
    if (m_hasCancel) m_btnCancel.draw(win);
}
bool UIMessageBox::confirmedOK() const { return m_ok; }
bool UIMessageBox::confirmedCancel() const { return m_cancel; }
void UIMessageBox::reset() { m_ok = m_cancel = false; }

// Splash Screen
void SplashScreen::onEnter(GUIContext& ctx) {
    m_timer = 0.f;
    m_alpha = 0.f;
    m_logo.setTexture(ctx.assets->getTexture(Assets::LOGO));
    float s = 200.f / m_logo.getLocalBounds().width;
    m_logo.setScale(s, s);
    m_logo.setPosition((1920.f - 200.f) / 2.f, 240.f);
    m_title.setFont(ctx.assets->getFont(true));
    m_title.setString("MEDICORE");
    m_title.setCharacterSize(72);
    m_title.setFillColor(sf::Color::White);
    sf::FloatRect tb = m_title.getLocalBounds();
    m_title.setPosition((1920.f - tb.width) / 2.f - tb.left, 460.f);
    m_subtitle.setFont(ctx.assets->getFont(false));
    m_subtitle.setString("Hospital Management System");
    m_subtitle.setCharacterSize(28);
    m_subtitle.setFillColor(Theme::TEXT_GRAY);
    sf::FloatRect sb = m_subtitle.getLocalBounds();
    m_subtitle.setPosition((1920.f - sb.width) / 2.f - sb.left, 550.f);
}
void SplashScreen::handleEvent(const sf::Event&, GUIContext&) {}
void SplashScreen::update(float dt, GUIContext& ctx) {
    m_timer += dt; m_alpha = (m_timer < 1.f) ? m_timer : 1.f;
    if (m_timer > 2.5f) ctx.nextScreen = ScreenID::LOGIN;
}
void SplashScreen::draw(sf::RenderWindow& win, GUIContext&) {
    win.clear(Theme::BG);
    sf::Uint8 a = (sf::Uint8)(m_alpha * 255);
    m_logo.setColor(sf::Color(255, 255, 255, a));
    m_title.setFillColor(sf::Color(255, 255, 255, a));
    m_subtitle.setFillColor(sf::Color(120, 140, 175, a));
    win.draw(m_logo);
    win.draw(m_title);
    win.draw(m_subtitle);
}

// Login Screen, centered layout
void LoginScreen::onEnter(GUIContext& ctx) {
    m_selectedRole = 0; m_failedAttempts = 0;
    // role tiles — centred horizontally
    float tileSize = 180.f, gap = 60.f;
    float totalW = 3 * tileSize + 2 * gap;
    float sx = (1920.f - totalW) / 2.f;
    float ty = 200.f;
    m_rolePatient.init(sx, ty, tileSize, "Patient", Assets::AVATAR_PATIENT, *ctx.assets);
    m_roleDoctor.init(sx + tileSize + gap, ty, tileSize, "Doctor", Assets::AVATAR_DOCTOR, *ctx.assets);
    m_roleAdmin.init(sx + 2 * (tileSize + gap), ty, tileSize, "Admin", Assets::AVATAR_ADMIN, *ctx.assets);

    float cx = 760.f, fw = 400.f, fh = 48.f;
    m_inputName.init(cx, 440.f, fw, fh, "Enter Name", *ctx.assets, false);
    m_inputPass.init(cx, 510.f, fw, fh, "Enter Password", *ctx.assets, true);
    m_btnLogin.init(cx + 80.f, 580.f, fw - 160.f, 48.f, "Login", *ctx.assets, true);
    m_btnBackRole.init(cx + 80.f, 645.f, fw - 160.f, 44.f, "Change Role", *ctx.assets, false);
    m_msgBox.init(*ctx.assets); m_msgBox.reset();

    m_header.setFont(ctx.assets->getFont(true));
    m_header.setString("Welcome to MediCore ");
    m_header.setCharacterSize(36);
    m_header.setFillColor(sf::Color::White);
    sf::FloatRect hb = m_header.getLocalBounds();
    m_header.setPosition((1920.f - hb.width) / 2.f - hb.left, 120.f);

    m_labelRole.setFont(ctx.assets->getFont(false));
    m_labelRole.setString(" ");
    m_labelRole.setCharacterSize(20);
    m_labelRole.setFillColor(Theme::TEXT_GRAY);
    m_labelRole.setPosition(sx, 174.f);

    m_attemptsText.setFont(ctx.assets->getFont(false));
    m_attemptsText.setCharacterSize(16);
    m_attemptsText.setFillColor(Theme::DANGER);
    m_attemptsText.setPosition(cx, 700.f);
}
void LoginScreen::handleEvent(const sf::Event& e, GUIContext& ctx) {
    sf::Vector2f mp = ctx.mousePos;
    m_rolePatient.handleEvent(e, *ctx.window, mp);
    m_roleDoctor.handleEvent(e, *ctx.window, mp);
    m_roleAdmin.handleEvent(e, *ctx.window, mp);
    m_inputName.handleEvent(e, *ctx.window, mp);
    m_inputPass.handleEvent(e, *ctx.window, mp);
    m_btnLogin.handleEvent(e, *ctx.window, mp);
    m_btnBackRole.handleEvent(e, *ctx.window, mp);
    m_msgBox.handleEvent(e, *ctx.window, mp);

    if (m_rolePatient.isClicked()) { m_selectedRole = 1; m_rolePatient.resetClick(); }
    if (m_roleDoctor.isClicked()) { m_selectedRole = 2; m_roleDoctor.resetClick(); }
    if (m_roleAdmin.isClicked()) { m_selectedRole = 3; m_roleAdmin.resetClick(); }

    if (m_btnBackRole.isClicked()) {
        m_btnBackRole.resetClick();
        m_selectedRole = 0; m_failedAttempts = 0;
        m_inputName.clear(); m_inputPass.clear();
    }

    if (m_btnLogin.isClicked()) {
        m_btnLogin.resetClick(); m_msgBox.reset();
        if (m_selectedRole == 0) { m_msgBox.show("Error", "Please select a role first."); return; }
        string nameStr = m_inputName.getText(), passStr = m_inputPass.getText();
        bool ok = false; int uid = 0; string uname; float bal = 0.f;
        if (m_selectedRole == 1) {
            Storage<Patient>& pats = FileHandler::getInstance()->loadPatients();
            for (int i = 0; i < pats.size(); i++) {
                Patient* p = pats.getAt(i);
                if (p && p->getName() == nameStr && p->checkPassword(passStr.c_str())) {
                    ok = true; uid = p->getID(); uname = p->getName(); bal = (float)p->getBalance(); break;
                }
            }
        }
        else if (m_selectedRole == 2) {
            Storage<Doctor>& docs = FileHandler::getInstance()->loadDoctors();
            for (int i = 0; i < docs.size(); i++) {
                Doctor* d = docs.getAt(i);
                if (d && d->getName() == nameStr && d->checkPassword(passStr.c_str())) {
                    ok = true; uid = d->getID(); uname = d->getName(); break;
                }
            }
        }
        else {
            Storage<Admin>& adms = FileHandler::getInstance()->loadAdmin();
            for (int i = 0; i < adms.size(); i++) {
                Admin* a = adms.getAt(i);
                if (a && a->getName() == nameStr && a->checkPassword(passStr.c_str())) {
                    ok = true; uid = a->getID(); uname = a->getName(); break;
                }
            }
        }
        const string roleStr = m_selectedRole == 1 ? "Patient" : m_selectedRole == 2 ? "Doctor" : "Admin";
        if (ok) {
            m_failedAttempts = 0;
            ctx.loggedInID = uid; ctx.role = m_selectedRole;
            ctx.patientBalance = bal; ctx.loggedInName = uname;
            m_inputName.clear(); m_inputPass.clear();
            FileHandler::getInstance()->logSecurityEvent(roleStr.c_str(), nameStr.c_str(), "SUCCESS");
            if (m_selectedRole == 1)      ctx.nextScreen = ScreenID::PATIENT_DASHBOARD;
            else if (m_selectedRole == 2) ctx.nextScreen = ScreenID::DOCTOR_DASHBOARD;
            else                       ctx.nextScreen = ScreenID::ADMIN_DASHBOARD;
        }
        else {
            m_failedAttempts++;
            FileHandler::getInstance()->logSecurityEvent(roleStr.c_str(), nameStr.c_str(), "FAILED");
            if (m_failedAttempts >= 3) {
                m_inputName.setReadOnly(true); m_inputPass.setReadOnly(true);
                m_btnLogin.setEnabled(false);
                m_msgBox.show("Account Locked", "Account locked. Contact admin.\nAttempt logged.");
                m_selectedRole = 0; m_failedAttempts = 0;
                m_inputName.clear(); m_inputPass.clear();
                m_inputName.setReadOnly(false); m_inputPass.setReadOnly(false);
                m_btnLogin.setEnabled(true);
            }
            else {
                m_msgBox.show("Login Failed", "Invalid credentials. Attempt " + intToStr(m_failedAttempts) + "/3");
            }
        }
    }
}
void LoginScreen::update(float, GUIContext&) {}
void LoginScreen::draw(sf::RenderWindow& win, GUIContext& ctx) {
    win.clear(Theme::BG);
    drawTopBar(win, ctx, "Pakistan Medical Center");
    win.draw(m_header);
    win.draw(m_labelRole);
    m_rolePatient.draw(win);
    m_roleDoctor.draw(win);
    m_roleAdmin.draw(win);
    if (m_selectedRole > 0) {
        m_inputName.draw(win);
        m_inputPass.draw(win);
        m_btnLogin.draw(win);
        m_btnBackRole.draw(win);
        if (m_failedAttempts > 0) {
            m_attemptsText.setString("Failed: " + intToStr(m_failedAttempts) + "/3");
            win.draw(m_attemptsText);
        }
    }
    m_msgBox.draw(win);
}

// Patient Dashboard, centered tiles
void PatientDashboard::onEnter(GUIContext& ctx) {
    const string labels[TILE_COUNT] = { "Book Appointment","My Appointments","Medical Records","My Bills","Pay Bill","Top Up Balance","Cancel Appt" };
    const string icons[TILE_COUNT] = { Assets::ICON_APPT,Assets::ICON_APPT,Assets::ICON_RECORDS,Assets::ICON_BILLING,Assets::ICON_BILLING,Assets::ICON_TOPUP,Assets::ICON_CANCEL };
    float tileSize = 200.f, gap = 28.f;
    float totalW = 4 * (tileSize + gap) - gap;
    float startX = (1920.f - totalW) / 2.f;
    float startY = 280.f;
    for (int i = 0; i < TILE_COUNT; i++) {
        int col = i % 4, row = i / 4;
        m_tiles[i].init(startX + col * (tileSize + gap), startY + row * (tileSize + gap + 20.f), tileSize, labels[i], icons[i], *ctx.assets);
    }
    m_btnLogout.init(1780.f, 15.f, 120.f, 42.f, "Logout", *ctx.assets, false);
    m_welcome.setFont(ctx.assets->getFont(true));
    m_welcome.setString("Welcome, " + ctx.loggedInName);
    m_welcome.setCharacterSize(22); m_welcome.setFillColor(sf::Color::White);
    m_welcome.setPosition(320.f, 20.f);
    m_balanceText.setFont(ctx.assets->getFont(false));
    m_balanceText.setCharacterSize(18); m_balanceText.setFillColor(sf::Color(100, 180, 255));
    m_balanceText.setPosition(82.f, 76.f);
}
void PatientDashboard::update(float, GUIContext& ctx) {
    m_balanceText.setString("Balance: PKR " + floatToStr(ctx.patientBalance));
}
void PatientDashboard::handleEvent(const sf::Event& e, GUIContext& ctx) {
    sf::Vector2f mp = ctx.mousePos;
    for (int i = 0; i < TILE_COUNT; i++) m_tiles[i].handleEvent(e, *ctx.window, mp);
    m_btnLogout.handleEvent(e, *ctx.window, mp);
    ScreenID targets[TILE_COUNT] = { ScreenID::PATIENT_BOOK_APPT,ScreenID::PATIENT_MY_APPTS,ScreenID::PATIENT_RECORDS,ScreenID::PATIENT_BILLS,ScreenID::PATIENT_PAY_BILL,ScreenID::PATIENT_TOPUP,ScreenID::PATIENT_MY_APPTS };
    for (int i = 0; i < TILE_COUNT; i++)
        if (m_tiles[i].isClicked()) { m_tiles[i].resetClick(); ctx.nextScreen = targets[i]; }
    if (m_btnLogout.isClicked()) { m_btnLogout.resetClick(); ctx.nextScreen = ScreenID::LOGIN; }
}
void PatientDashboard::draw(sf::RenderWindow& win, GUIContext& ctx) {
    win.clear(Theme::BG);
    drawTopBar(win, ctx, ctx.loggedInName);
    win.draw(m_welcome);
    sf::RectangleShape sub(sf::Vector2f(1920.f, 32.f));
    sub.setPosition(0.f, 70.f);
    sub.setFillColor(sf::Color(6, 18, 44));
    win.draw(sub);
    win.draw(m_balanceText);
    for (int i = 0; i < TILE_COUNT; i++) m_tiles[i].draw(win);
    m_btnLogout.draw(win);
}

// Patient Book Appointment, table row click selects doctor/slot
void PatientBookAppt::onEnter(GUIContext& ctx) {
    m_selectedDocID = -1; m_selectedDate = ""; m_selectedSlot = "";
    m_title.setFont(ctx.assets->getFont(true));
    m_title.setString("Book Appointment");
    m_title.setCharacterSize(28); m_title.setFillColor(sf::Color::White);
    m_title.setPosition(60.f, 82.f);

    m_inputSpec.init(60.f, 125.f, 340.f, 46.f, "Enter specialization...", *ctx.assets);
    m_btnSearch.init(412.f, 125.f, 130.f, 46.f, "Search", *ctx.assets);

    string docHdrs[] = { "ID","Name","Specialization","Fee (PKR)" };
    m_tableDoc.init(60.f, 192.f, 820.f, 200.f, docHdrs, 4, *ctx.assets);

    // label hints
    m_lblSelectedDoc.setFont(ctx.assets->getFont(false));
    m_lblSelectedDoc.setCharacterSize(16);
    m_lblSelectedDoc.setFillColor(Theme::SUCCESS);
    m_lblSelectedDoc.setPosition(60.f, 400.f);

    m_inputDate.init(60.f, 430.f, 240.f, 46.f, "DD-MM-YYYY", *ctx.assets);
    m_btnLoadSlots.init(314.f, 430.f, 160.f, 46.f, "Show Slots", *ctx.assets);

    string slotHdrs[] = { "Time Slot","Status" };
    m_tableSlots.init(60.f, 490.f, 360.f, 270.f, slotHdrs, 2, *ctx.assets);

    m_lblSelectedSlot.setFont(ctx.assets->getFont(false));
    m_lblSelectedSlot.setCharacterSize(16);
    m_lblSelectedSlot.setFillColor(Theme::SUCCESS);
    m_lblSelectedSlot.setPosition(60.f, 768.f);

    m_btnBook.init(60.f, 800.f, 200.f, 50.f, "Confirm Booking", *ctx.assets);
    m_btnBack.init(60.f, 866.f, 140.f, 44.f, "Back", *ctx.assets, false);

    m_statusText.setFont(ctx.assets->getFont(false));
    m_statusText.setCharacterSize(15);
    m_statusText.setFillColor(Theme::DANGER);
    m_statusText.setPosition(60.f, 785.f);

    m_msgBox.init(*ctx.assets);
}
void PatientBookAppt::loadDoctors(const string& spec) {
    m_tableDoc.clearRows();
    Storage<Doctor>& docs = FileHandler::getInstance()->loadDoctors();
    bool found = false;
    for (int i = 0; i < docs.size(); i++) {
        Doctor* d = docs.getAt(i);
        if (!d) continue;
        if (d->matchesSpec(spec.c_str())) {
            string row[] = { intToStr(d->getID()),d->getName(),d->getSpec(),floatToStr(d->getFee()) };
            m_tableDoc.addRow(row); found = true;
        }
    }
    m_statusText.setString(found ? "Click a doctor row to select it." : "No doctors found for that specialization.");
}
void PatientBookAppt::loadSlots(int docID, const string& date) {
    m_tableSlots.clearRows();
    const string slots[8] = { "09:00","10:00","11:00","12:00","13:00","14:00","15:00","16:00" };
    Storage<Appointment>& appts = FileHandler::getInstance()->loadAppointments();
    for (int i = 0; i < 8; i++) {
        bool taken = false;
        for (int j = 0; j < appts.size(); j++) {
            Appointment* a = appts.getAt(j);
            Appointment check(0, 0, docID, date, slots[i], "pending");
            if (*a == check) { taken = true; break; }
        }
        string row[] = { slots[i],taken ? "Taken" : "Available" };
        m_tableSlots.addRow(row);
    }
    m_statusText.setString("Click an Available slot to select it.");
}
void PatientBookAppt::handleEvent(const sf::Event& e, GUIContext& ctx) {
    sf::Vector2f mp = ctx.mousePos;
    m_inputSpec.handleEvent(e, *ctx.window, mp);
    m_btnSearch.handleEvent(e, *ctx.window, mp);
    m_tableDoc.handleEvent(e, mp);
    m_inputDate.handleEvent(e, *ctx.window, mp);
    m_btnLoadSlots.handleEvent(e, *ctx.window, mp);
    m_tableSlots.handleEvent(e, mp);
    m_btnBook.handleEvent(e, *ctx.window, mp);
    m_btnBack.handleEvent(e, *ctx.window, mp);
    m_msgBox.handleEvent(e, *ctx.window, mp);

    // search doctors
    if (m_btnSearch.isClicked()) {
        m_btnSearch.resetClick();
        m_selectedDocID = -1; m_selectedSlot = "";
        m_tableSlots.clearRows();
        m_lblSelectedDoc.setString(""); m_lblSelectedSlot.setString("");
        loadDoctors(m_inputSpec.getText());
    }

    // doctor row selection — only react to a NEW doctor choice
    if (m_tableDoc.getSelectedRow() >= 0) {
        string idStr = m_tableDoc.getSelectedCell(0);
        if (!idStr.empty()) {
            int newDocID = strToInt(idStr);
            if (newDocID != m_selectedDocID) {
                m_selectedDocID = newDocID;
                string docName = m_tableDoc.getSelectedCell(1);
                m_lblSelectedDoc.setString("Selected: " + docName + " (ID " + idStr + ")");
                m_selectedSlot = "";
                m_lblSelectedSlot.setString("");
                // auto-load slots if date is already filled
                if (!m_inputDate.getText().empty() &&
                    Validator::isValidDate(m_inputDate.getText()))
                    loadSlots(m_selectedDocID, m_inputDate.getText());
            }
        }
    }

    // show slots button
    if (m_btnLoadSlots.isClicked()) {
        m_btnLoadSlots.resetClick();
        if (m_selectedDocID < 0) {
            m_statusText.setString("Please select a doctor first."); return;
        }
        if (!Validator::isValidDate(m_inputDate.getText())) {
            m_statusText.setString("Invalid date. Use DD-MM-YYYY."); return;
        }
        loadSlots(m_selectedDocID, m_inputDate.getText());
    }

    // slot row selection — only pick "Available" rows
    if (m_tableSlots.getSelectedRow() >= 0) {
        string status = m_tableSlots.getSelectedCell(1);
        if (status == "Available") {
            m_selectedSlot = m_tableSlots.getSelectedCell(0);
            m_lblSelectedSlot.setString("Selected slot: " + m_selectedSlot);
            m_statusText.setString("");
        }
        else {
            m_selectedSlot = "";
            m_lblSelectedSlot.setString("That slot is taken, choose another.");
        }
    }

    // book button
    if (m_btnBook.isClicked()) {
        m_btnBook.resetClick();
        try {
            if (m_selectedDocID < 0)
                throw InvalidInputException("Please select a doctor from the list.");
            if (!Validator::isValidDate(m_inputDate.getText()))
                throw InvalidInputException("Invalid date. Use DD-MM-YYYY.");
            if (m_selectedSlot.empty())
                throw InvalidInputException("Please select an available time slot.");

            m_selectedDate = m_inputDate.getText();

            Storage<Appointment>& appts = FileHandler::getInstance()->loadAppointments();
            for (int j = 0; j < appts.size(); j++) {
                Appointment* a = appts.getAt(j);
                Appointment check(0, 0, m_selectedDocID, m_selectedDate, m_selectedSlot, "pending");
                if (*a == check) throw SlotUnavailableException("That slot is no longer available.");
            }
            Patient* p = FileHandler::getInstance()->loadPatients().findByID(ctx.loggedInID);
            Doctor* d = FileHandler::getInstance()->loadDoctors().findByID(m_selectedDocID);
            if (!p || !d) throw InvalidInputException("Patient or doctor not found.");
            if (p->getBalance() < d->getFee())
                throw InsufficientFundsException("Insufficient balance. Please top up.");

            *p -= d->getFee();
            int newApptID = FileHandler::getInstance()->getMaxAppointmentID() + 1;
            int newBillID = FileHandler::getInstance()->getMaxBillID() + 1;
            Appointment newAppt(newApptID, p->getID(), d->getID(), m_selectedDate, m_selectedSlot, "pending");
            Bill newBill(newBillID, p->getID(), newApptID, d->getFee(), "unpaid", m_selectedDate);
            FileHandler::getInstance()->appendAppointment(newAppt);
            FileHandler::getInstance()->appendBill(newBill);
            FileHandler::getInstance()->updatePatient(*p);
            ctx.patientBalance = (float)p->getBalance();
            m_msgBox.show("Success", "Appointment booked! ID: " + intToStr(newApptID));
            // reset selections
            m_selectedDocID = -1; m_selectedSlot = "";
            m_lblSelectedDoc.setString(""); m_lblSelectedSlot.setString("");
            m_tableDoc.clearRows(); m_tableSlots.clearRows();
        }
        catch (SlotUnavailableException& ex) { m_statusText.setString(ex.what()); loadSlots(m_selectedDocID, m_selectedDate); }
        catch (InsufficientFundsException& ex) { m_msgBox.show("Insufficient Funds", ex.what()); }
        catch (InvalidInputException& ex) { m_msgBox.show("Validation Error", ex.what()); }
    }
    if (m_btnBack.isClicked()) { m_btnBack.resetClick(); ctx.nextScreen = ScreenID::PATIENT_DASHBOARD; }
}
void PatientBookAppt::update(float, GUIContext&) {}
void PatientBookAppt::draw(sf::RenderWindow& win, GUIContext& ctx) {
    win.clear(Theme::BG);
    drawTopBar(win, ctx, ctx.loggedInName);
    win.draw(m_title);
    m_inputSpec.draw(win);
    m_btnSearch.draw(win);
    m_tableDoc.draw(win);
    win.draw(m_lblSelectedDoc);
    m_inputDate.draw(win);
    m_btnLoadSlots.draw(win);
    m_tableSlots.draw(win);
    win.draw(m_lblSelectedSlot);
    win.draw(m_statusText);
    m_btnBook.draw(win);
    m_btnBack.draw(win);
    m_msgBox.draw(win);
}

// Patient My Appointments
void PatientMyAppts::onEnter(GUIContext& ctx) {
    m_title.setFont(ctx.assets->getFont(true));
    m_title.setString("My Appointments");
    m_title.setCharacterSize(28);
    m_title.setFillColor(sf::Color::White);
    m_title.setPosition(60.f, 82.f);
    string hdrs[] = { "ID","Doctor","Specialization","Date","Slot","Status" };
    m_table.init(60.f, 126.f, 1800.f, 500.f, hdrs, 6, *ctx.assets);
    refreshTable(ctx);
    m_inputID.init(60.f, 646.f, 240.f, 44.f, "Appointment ID", *ctx.assets);
    m_btnCancel.init(316.f, 646.f, 180.f, 44.f, "Cancel Appt", *ctx.assets);
    m_btnBack.init(60.f, 706.f, 140.f, 42.f, "Back", *ctx.assets, false);
    m_msgBox.init(*ctx.assets);
}
void PatientMyAppts::refreshTable(GUIContext& ctx) {
    m_table.clearRows();
    Storage<Appointment>& appts = FileHandler::getInstance()->loadAppointments();
    static Appointment* arr[200]; int cnt = 0;
    for (int i = 0; i < appts.size() && cnt < 200; i++) {
        Appointment* a = appts.getAt(i);
        if (a && a->getPatientID() == ctx.loggedInID) arr[cnt++] = a;
    }
    for (int i = 0; i < cnt - 1; i++)
        for (int j = 0; j < cnt - i - 1; j++)
            if (Validator::dateToInt(arr[j]->getDate()) > Validator::dateToInt(arr[j + 1]->getDate())) {
                Appointment* t = arr[j]; arr[j] = arr[j + 1]; arr[j + 1] = t;
            }
    for (int i = 0; i < cnt; i++) {
        Doctor* d = FileHandler::getInstance()->loadDoctors().findByID(arr[i]->getDoctorID());
        string row[] = { intToStr(arr[i]->getID()),d ? string(d->getName()) : "Unknown",d ? string(d->getSpec()) : "-",arr[i]->getDate(),arr[i]->getTimeSlot(),arr[i]->getStatus() };
        m_table.addRow(row);
    }
}
void PatientMyAppts::handleEvent(const sf::Event& e, GUIContext& ctx) {
    sf::Vector2f mp = ctx.mousePos;
    m_table.handleEvent(e, mp);
    m_inputID.handleEvent(e, *ctx.window, mp);
    m_btnCancel.handleEvent(e, *ctx.window, mp);
    m_btnBack.handleEvent(e, *ctx.window, mp);
    m_msgBox.handleEvent(e, *ctx.window, mp);
    if (m_btnCancel.isClicked()) { m_btnCancel.resetClick(); m_msgBox.show("Confirm", "Cancel this appointment?", true); }
    if (m_msgBox.confirmedOK()) {
        m_msgBox.reset();
        try {
            int apptID = strToInt(m_inputID.getText());
            Appointment* a = FileHandler::getInstance()->loadAppointments().findByID(apptID);
            if (!a || a->getPatientID() != ctx.loggedInID || !Validator::stringEqual(a->getStatus(), "pending")) {
                m_msgBox.show("Error", "Invalid appointment ID."); return;
            }
            Doctor* d = FileHandler::getInstance()->loadDoctors().findByID(a->getDoctorID());
            Patient* p = FileHandler::getInstance()->loadPatients().findByID(ctx.loggedInID);
            if (d)*p += d->getFee();
            a->setStatus("cancelled");
            FileHandler::getInstance()->updateAppointment(*a);
            FileHandler::getInstance()->updatePatient(*p);
            Storage<Bill>& bills = FileHandler::getInstance()->loadBills();
            for (int i = 0; i < bills.size(); i++) {
                Bill* b = bills.getAt(i);
                if (b && b->getAppointmentID() == apptID) { b->setStatus("cancelled"); FileHandler::getInstance()->updateBill(*b); break; }
            }
            ctx.patientBalance = (float)p->getBalance();
            string msg = "Appointment cancelled.";
            if (d) msg += " PKR " + floatToStr(d->getFee()) + " refunded.";
            m_msgBox.show("Done", msg);
            refreshTable(ctx);
        }
        catch (HospitalException& ex) { m_msgBox.show("Error", ex.what()); }
    }
    if (m_btnBack.isClicked()) { m_btnBack.resetClick(); ctx.nextScreen = ScreenID::PATIENT_DASHBOARD; }
}
void PatientMyAppts::update(float, GUIContext&) {}
void PatientMyAppts::draw(sf::RenderWindow& win, GUIContext& ctx) {
    win.clear(Theme::BG);
    drawTopBar(win, ctx, ctx.loggedInName);
    win.draw(m_title);
    m_table.draw(win);
    m_inputID.draw(win);
    m_btnCancel.draw(win);
    m_btnBack.draw(win);
    m_msgBox.draw(win);
}

// Patient Records
void PatientRecords::onEnter(GUIContext& ctx) {
    m_title.setFont(ctx.assets->getFont(true));
    m_title.setString("My Medical Records");
    m_title.setCharacterSize(28);
    m_title.setFillColor(sf::Color::White);
    m_title.setPosition(60.f, 82.f);
    string hdrs[] = { "Date","Doctor","Medicines","Notes" };
    m_table.init(60.f, 126.f, 1800.f, 600.f, hdrs, 4, *ctx.assets);
    Storage<Prescription>& prescs = FileHandler::getInstance()->loadPrescriptions();
    static Prescription* arr[200]; int cnt = 0;
    for (int i = 0; i < prescs.size() && cnt < 200; i++) {
        Prescription* pr = prescs.getAt(i);
        if (pr && pr->getPatientID() == ctx.loggedInID) arr[cnt++] = pr;
    }
    for (int i = 0; i < cnt - 1; i++)
        for (int j = 0; j < cnt - i - 1; j++)
            if (Validator::dateToInt(arr[j]->getDate()) < Validator::dateToInt(arr[j + 1]->getDate())) {
                Prescription* t = arr[j]; arr[j] = arr[j + 1]; arr[j + 1] = t;
            }
    for (int i = 0; i < cnt; i++) {
        Doctor* d = FileHandler::getInstance()->loadDoctors().findByID(arr[i]->getDoctorID());
        string row[] = { arr[i]->getDate(),d ? string(d->getName()) : "Unknown",arr[i]->getMedicines(),arr[i]->getNotes() };
        m_table.addRow(row);
    }
    m_btnBack.init(60.f, 750.f, 140.f, 42.f, "Back", *ctx.assets, false);
}
void PatientRecords::handleEvent(const sf::Event& e, GUIContext& ctx) {
    sf::Vector2f mp = ctx.mousePos;
    m_table.handleEvent(e, mp); m_btnBack.handleEvent(e, *ctx.window, mp);
    if (m_btnBack.isClicked()) { m_btnBack.resetClick(); ctx.nextScreen = ScreenID::PATIENT_DASHBOARD; }
}
void PatientRecords::update(float, GUIContext&) {}
void PatientRecords::draw(sf::RenderWindow& win, GUIContext& ctx) {
    win.clear(Theme::BG);
    drawTopBar(win, ctx, ctx.loggedInName);
    win.draw(m_title);
    m_table.draw(win);
    m_btnBack.draw(win);
}

// Patient Bills
void PatientBills::onEnter(GUIContext& ctx) {
    m_title.setFont(ctx.assets->getFont(true));
    m_title.setString("My Bills");
    m_title.setCharacterSize(28);
    m_title.setFillColor(sf::Color::White);
    m_title.setPosition(60.f, 82.f);
    string hdrs[] = { "Bill ID","Appt ID","Amount (PKR)","Status","Date" };
    m_table.init(60.f, 126.f, 1800.f, 450.f, hdrs, 5, *ctx.assets);
    float total = 0.f;
    Storage<Bill>& bills = FileHandler::getInstance()->loadBills();
    for (int i = 0; i < bills.size(); i++) {
        Bill* b = bills.getAt(i);
        if (!b || b->getPatientID() != ctx.loggedInID) continue;
        string row[] = { intToStr(b->getID()),intToStr(b->getApptID()),floatToStr(b->getAmount()),b->getStatus(),b->getDate() };
        m_table.addRow(row);
        if (Validator::stringEqual(b->getStatus(), "unpaid")) total += b->getAmount();
    }
    m_totalText.setFont(ctx.assets->getFont(true)); m_totalText.setCharacterSize(18);
    m_totalText.setFillColor(Theme::DANGER); m_totalText.setPosition(1400.f, 596.f);
    m_totalText.setString("Total Outstanding: PKR " + floatToStr(total));
    m_inputBillID.init(60.f, 596.f, 240.f, 46.f, "Bill ID", *ctx.assets);
    m_btnPay.init(312.f, 596.f, 140.f, 46.f, "Pay Bill", *ctx.assets);
    m_btnTopUp.init(464.f, 596.f, 140.f, 46.f, "Top Up", *ctx.assets, false);
    m_btnBack.init(60.f, 658.f, 140.f, 42.f, "Back", *ctx.assets, false);
    m_msgBox.init(*ctx.assets);
}
void PatientBills::handleEvent(const sf::Event& e, GUIContext& ctx) {
    sf::Vector2f mp = ctx.mousePos;
    m_table.handleEvent(e, mp);
    m_inputBillID.handleEvent(e, *ctx.window, mp);
    m_btnPay.handleEvent(e, *ctx.window, mp);
    m_btnTopUp.handleEvent(e, *ctx.window, mp);
    m_btnBack.handleEvent(e, *ctx.window, mp);
    m_msgBox.handleEvent(e, *ctx.window, mp);
    if (m_btnPay.isClicked()) {
        m_btnPay.resetClick();
        try {
            int billID = strToInt(m_inputBillID.getText());
            Bill* b = FileHandler::getInstance()->loadBills().findByID(billID);
            if (!b || b->getPatientID() != ctx.loggedInID || !Validator::stringEqual(b->getStatus(), "unpaid")) {
                m_msgBox.show("Error", "Invalid or already paid bill ID."); return;
            }
            Patient* p = FileHandler::getInstance()->loadPatients().findByID(ctx.loggedInID);
            if (p->getBalance() < b->getAmount()) throw InsufficientFundsException("Insufficient balance.");
            *p -= b->getAmount(); b->setStatus("paid");
            FileHandler::getInstance()->updateBill(*b); FileHandler::getInstance()->updatePatient(*p);
            ctx.patientBalance = (float)p->getBalance();
            m_msgBox.show("Success", "Bill paid. Remaining: PKR " + floatToStr((float)p->getBalance()));
        }
        catch (InsufficientFundsException& ex) { m_msgBox.show("Error", ex.what()); }
    }
    if (m_btnTopUp.isClicked()) { m_btnTopUp.resetClick(); ctx.nextScreen = ScreenID::PATIENT_TOPUP; }
    if (m_btnBack.isClicked()) { m_btnBack.resetClick(); ctx.nextScreen = ScreenID::PATIENT_DASHBOARD; }
}
void PatientBills::update(float, GUIContext&) {}
void PatientBills::draw(sf::RenderWindow& win, GUIContext& ctx) {
    win.clear(Theme::BG);
    drawTopBar(win, ctx, ctx.loggedInName);
    win.draw(m_title);
    m_table.draw(win);
    m_inputBillID.draw(win);
    m_btnPay.draw(win);
    m_btnTopUp.draw(win);
    m_btnBack.draw(win);
    win.draw(m_totalText);
    m_msgBox.draw(win);
}

// Patient Top Up Balance
void PatientTopUp::onEnter(GUIContext& ctx) {
    float cx = 760.f;
    m_title.setFont(ctx.assets->getFont(true));
    m_title.setString("Top Up Balance");
    m_title.setCharacterSize(28);
    m_title.setFillColor(sf::Color::White);
    m_title.setPosition(cx, 200.f);
    m_currentBalance.setFont(ctx.assets->getFont(false)); m_currentBalance.setCharacterSize(20);
    m_currentBalance.setFillColor(Theme::TEXT_GRAY); m_currentBalance.setPosition(cx, 248.f);
    m_currentBalance.setString("Current Balance: PKR " + floatToStr(ctx.patientBalance));
    m_inputAmount.init(cx, 310.f, 400.f, 48.f, "Amount (PKR)", *ctx.assets);
    m_btnAdd.init(cx + 100.f, 374.f, 200.f, 48.f, "Add Funds", *ctx.assets);
    m_btnBack.init(cx + 100.f, 436.f, 200.f, 44.f, "Back", *ctx.assets, false);
    m_msgBox.init(*ctx.assets);
}
void PatientTopUp::handleEvent(const sf::Event& e, GUIContext& ctx) {
    sf::Vector2f mp = ctx.mousePos;
    m_inputAmount.handleEvent(e, *ctx.window, mp);
    m_btnAdd.handleEvent(e, *ctx.window, mp); m_btnBack.handleEvent(e, *ctx.window, mp);
    m_msgBox.handleEvent(e, *ctx.window, mp);
    if (m_btnAdd.isClicked()) {
        m_btnAdd.resetClick();
        try {
            if (!Validator::isPositiveFloat(m_inputAmount.getText().c_str()))
                throw InvalidInputException("Amount must be a positive number.");
            float amount = strToFloat(m_inputAmount.getText());
            Patient* p = FileHandler::getInstance()->loadPatients().findByID(ctx.loggedInID);
            *p += amount; FileHandler::getInstance()->updatePatient(*p);
            ctx.patientBalance = (float)p->getBalance();
            m_currentBalance.setString("Current Balance: PKR " + floatToStr((float)p->getBalance()));
            m_msgBox.show("Success", "Balance updated. New balance: PKR " + floatToStr((float)p->getBalance()));
        }
        catch (InvalidInputException& ex) { m_msgBox.show("Validation Error", ex.what()); }
    }
    if (m_btnBack.isClicked()) { m_btnBack.resetClick(); ctx.nextScreen = ScreenID::PATIENT_DASHBOARD; }
}
void PatientTopUp::update(float, GUIContext&) {}
void PatientTopUp::draw(sf::RenderWindow& win, GUIContext& ctx) {
    win.clear(Theme::BG);
    drawTopBar(win, ctx, ctx.loggedInName);
    drawCard(win, 700.f, 170.f, 520.f, 350.f, sf::Color(18, 26, 46), sf::Color(30, 100, 200), 2.f);
    win.draw(m_title);
    win.draw(m_currentBalance);
    m_inputAmount.draw(win);
    m_btnAdd.draw(win);
    m_btnBack.draw(win);
    m_msgBox.draw(win);
}

// Doctor Dashboard
void DoctorDashboard::onEnter(GUIContext& ctx) {
    const string labels[TILE_COUNT] = { "Today's Appointments","Mark Complete","Write Prescription","Patient History","Mark No-Show" };
    const string icons[TILE_COUNT] = { Assets::ICON_APPT,Assets::ICON_APPT,Assets::ICON_PRESC,Assets::ICON_PATIENT,Assets::ICON_CANCEL };
    float tileSize = 220.f, gap = 36.f, totalW = 3 * (tileSize + gap) - gap;
    float sx = (1920.f - totalW) / 2.f, sy = 260.f;
    for (int i = 0; i < TILE_COUNT; i++) {
        int c = i % 3, r = i / 3;
        m_tiles[i].init(sx + c * (tileSize + gap), sy + r * (tileSize + gap + 20.f), tileSize, labels[i], icons[i], *ctx.assets);
    }
    m_btnLogout.init(1780.f, 15.f, 120.f, 42.f, "Logout", *ctx.assets, false);
    m_welcome.setFont(ctx.assets->getFont(true));
    m_welcome.setString("Dr. " + ctx.loggedInName);
    m_welcome.setCharacterSize(22); m_welcome.setFillColor(sf::Color::White); m_welcome.setPosition(320.f, 20.f);
}
void DoctorDashboard::handleEvent(const sf::Event& e, GUIContext& ctx) {
    sf::Vector2f mp = ctx.mousePos;
    for (int i = 0; i < TILE_COUNT; i++) m_tiles[i].handleEvent(e, *ctx.window, mp);
    m_btnLogout.handleEvent(e, *ctx.window, mp);
    ScreenID targets[TILE_COUNT] = { ScreenID::DOCTOR_TODAY_APPTS,ScreenID::DOCTOR_MARK_COMPLETE,ScreenID::DOCTOR_WRITE_PRESC,ScreenID::DOCTOR_PATIENT_HISTORY,ScreenID::DOCTOR_TODAY_APPTS };
    for (int i = 0; i < TILE_COUNT; i++)
        if (m_tiles[i].isClicked()) { m_tiles[i].resetClick(); ctx.nextScreen = targets[i]; }
    if (m_btnLogout.isClicked()) { m_btnLogout.resetClick(); ctx.nextScreen = ScreenID::LOGIN; }
}
void DoctorDashboard::update(float, GUIContext&) {}
void DoctorDashboard::draw(sf::RenderWindow& win, GUIContext& ctx) {
    win.clear(Theme::BG);
    drawTopBar(win, ctx, ctx.loggedInName);
    win.draw(m_welcome);
    for (int i = 0; i < TILE_COUNT; i++) m_tiles[i].draw(win);
    m_btnLogout.draw(win);
}

// Doctor Today Appointments
void DoctorTodayAppts::onEnter(GUIContext& ctx) {
    m_title.setFont(ctx.assets->getFont(true));
    m_title.setString("Today's Appointments");
    m_title.setCharacterSize(28);
    m_title.setFillColor(sf::Color::White);
    m_title.setPosition(60.f, 82.f);
    string hdrs[] = { "Appt ID","Patient","Time Slot","Status" };
    m_table.init(60.f, 126.f, 1800.f, 480.f, hdrs, 4, *ctx.assets);
    string today = getTodayStr();
    Storage<Appointment>& appts = FileHandler::getInstance()->loadAppointments();
    static Appointment* arr[50]; int cnt = 0;
    for (int i = 0; i < appts.size() && cnt < 50; i++) {
        Appointment* a = appts.getAt(i);
        if (a && a->getDoctorID() == ctx.loggedInID && Validator::stringEqual(a->getDate(), today)) arr[cnt++] = a;
    }
    for (int i = 0; i < cnt - 1; i++)
        for (int j = 0; j < cnt - i - 1; j++)
            if (arr[j]->getTimeSlot() > arr[j + 1]->getTimeSlot()) { Appointment* t = arr[j]; arr[j] = arr[j + 1]; arr[j + 1] = t; }
    for (int i = 0; i < cnt; i++) {
        Patient* p = FileHandler::getInstance()->loadPatients().findByID(arr[i]->getPatientID());
        string row[] = { intToStr(arr[i]->getID()),p ? string(p->getName()) : "Unknown",arr[i]->getTimeSlot(),arr[i]->getStatus() };
        m_table.addRow(row);
    }
    m_inputID.init(60.f, 626.f, 240.f, 46.f, "Appointment ID", *ctx.assets);
    m_btnComplete.init(314.f, 626.f, 180.f, 46.f, "Mark Complete", *ctx.assets);
    m_btnNoShow.init(508.f, 626.f, 140.f, 46.f, "No-Show", *ctx.assets);
    m_btnBack.init(60.f, 688.f, 140.f, 42.f, "Back", *ctx.assets, false);
    m_msgBox.init(*ctx.assets);
}
void DoctorTodayAppts::handleEvent(const sf::Event& e, GUIContext& ctx) {
    sf::Vector2f mp = ctx.mousePos;
    m_table.handleEvent(e, mp);
    m_inputID.handleEvent(e, *ctx.window, mp);
    m_btnComplete.handleEvent(e, *ctx.window, mp);
    m_btnNoShow.handleEvent(e, *ctx.window, mp);
    m_btnBack.handleEvent(e, *ctx.window, mp);
    m_msgBox.handleEvent(e, *ctx.window, mp);
    string today = getTodayStr();
    if (m_btnComplete.isClicked()) {
        m_btnComplete.resetClick();
        int apptID = strToInt(m_inputID.getText());
        Appointment* a = FileHandler::getInstance()->loadAppointments().findByID(apptID);
        if (a && a->getDoctorID() == ctx.loggedInID && Validator::stringEqual(a->getStatus(), "pending") && Validator::stringEqual(a->getDate(), today)) {
            a->setStatus("completed"); FileHandler::getInstance()->updateAppointment(*a);
            m_msgBox.show("Done", "Appointment marked as completed.");
        }
        else m_msgBox.show("Error", "Invalid appointment ID or not pending/today.");
    }
    if (m_btnNoShow.isClicked()) {
        m_btnNoShow.resetClick();
        int apptID = strToInt(m_inputID.getText());
        Appointment* a = FileHandler::getInstance()->loadAppointments().findByID(apptID);
        if (a && a->getDoctorID() == ctx.loggedInID && Validator::stringEqual(a->getStatus(), "pending") && Validator::stringEqual(a->getDate(), today)) {
            a->setStatus("noshow"); FileHandler::getInstance()->updateAppointment(*a);
            Storage<Bill>& bills = FileHandler::getInstance()->loadBills();
            for (int i = 0; i < bills.size(); i++) {
                Bill* b = bills.getAt(i);
                if (b && b->getAppointmentID() == apptID) { b->setStatus("cancelled"); FileHandler::getInstance()->updateBill(*b); break; }
            }
            m_msgBox.show("Done", "Appointment marked as no-show.");
        }
        else m_msgBox.show("Error", "Invalid appointment ID or not pending/today.");
    }
    if (m_btnBack.isClicked()) { m_btnBack.resetClick(); ctx.nextScreen = ScreenID::DOCTOR_DASHBOARD; }
}
void DoctorTodayAppts::update(float, GUIContext&) {}
void DoctorTodayAppts::draw(sf::RenderWindow& win, GUIContext& ctx) {
    win.clear(Theme::BG);
    drawTopBar(win, ctx, ctx.loggedInName);
    win.draw(m_title);
    m_table.draw(win);
    m_inputID.draw(win);
    m_btnComplete.draw(win);
    m_btnNoShow.draw(win);
    m_btnBack.draw(win);
    m_msgBox.draw(win);
}

// Doctor Write Prescription
void DoctorWritePresc::onEnter(GUIContext& ctx) {
    // card: 480x400, centered on 1920x(1080-70)
    float cardW = 480.f, cardH = 400.f;
    float cardX = (1920.f - cardW) / 2.f;           // 720
    float cardY = 70.f + (1010.f - cardH) / 2.f;    // 375
    float cx = cardX + 40.f;                         // 760 — input left
    m_title.setFont(ctx.assets->getFont(true));
    m_title.setString("Write Prescription");
    m_title.setCharacterSize(28);
    m_title.setFillColor(sf::Color::White);
    m_title.setPosition(cx, cardY + 20.f);
    m_inputApptID.init(cx, cardY + 75.f, 400.f, 46.f, "Appointment ID", *ctx.assets);
    m_inputMeds.init(cx, cardY + 138.f, 400.f, 46.f, "Medicines (Para 500mg;Amox 250mg)", *ctx.assets);
    m_inputNotes.init(cx, cardY + 201.f, 400.f, 46.f, "Notes (max 300 chars)", *ctx.assets);
    m_btnSave.init(cx + 100.f, cardY + 266.f, 200.f, 48.f, "Save", *ctx.assets);
    m_btnBack.init(cx + 100.f, cardY + 328.f, 200.f, 44.f, "Back", *ctx.assets, false);
    m_msgBox.init(*ctx.assets);
}
void DoctorWritePresc::handleEvent(const sf::Event& e, GUIContext& ctx) {
    sf::Vector2f mp = ctx.mousePos;
    m_inputApptID.handleEvent(e, *ctx.window, mp);
    m_inputMeds.handleEvent(e, *ctx.window, mp);
    m_inputNotes.handleEvent(e, *ctx.window, mp);
    m_btnSave.handleEvent(e, *ctx.window, mp); m_btnBack.handleEvent(e, *ctx.window, mp);
    m_msgBox.handleEvent(e, *ctx.window, mp);
    if (m_btnSave.isClicked()) {
        m_btnSave.resetClick();
        try {
            int apptID = strToInt(m_inputApptID.getText());
            Appointment* a = FileHandler::getInstance()->loadAppointments().findByID(apptID);
            if (!a || a->getDoctorID() != ctx.loggedInID) throw InvalidInputException("Appointment not found or not yours.");
            if (!Validator::stringEqual(a->getStatus(), "completed")) throw InvalidInputException("Appointment not completed yet.");
            if (FileHandler::getInstance()->prescriptionExists(apptID)) throw InvalidInputException("Prescription already written.");
            string meds = m_inputMeds.getText(), notes = m_inputNotes.getText();
            if (meds.size() > 499) meds = meds.substr(0, 499);
            if (notes.size() > 299) notes = notes.substr(0, 299);
            int newID = FileHandler::getInstance()->getMaxPrescID() + 1;
            Prescription presc(newID, apptID, a->getPatientID(), ctx.loggedInID, getTodayStr(), meds, notes);
            FileHandler::getInstance()->appendPrescription(presc);
            m_msgBox.show("Saved", "Prescription saved successfully.");
            m_inputApptID.clear(); m_inputMeds.clear(); m_inputNotes.clear();
        }
        catch (HospitalException& ex) { m_msgBox.show("Error", ex.what()); }
    }
    if (m_btnBack.isClicked()) { m_btnBack.resetClick(); ctx.nextScreen = ScreenID::DOCTOR_DASHBOARD; }
}
void DoctorWritePresc::update(float, GUIContext&) {}
void DoctorWritePresc::draw(sf::RenderWindow& win, GUIContext& ctx) {
    win.clear(Theme::BG);
    drawTopBar(win, ctx, ctx.loggedInName);
    drawCard(win, 720.f, 375.f, 480.f, 400.f, sf::Color(18, 26, 46), sf::Color(30, 100, 200), 2.f);
    win.draw(m_title);
    m_inputApptID.draw(win);
    m_inputMeds.draw(win);
    m_inputNotes.draw(win);
    m_btnSave.draw(win);
    m_btnBack.draw(win);
    m_msgBox.draw(win);
}

// Doctor Patient History
void DoctorPatientHistory::onEnter(GUIContext& ctx) {
    m_title.setFont(ctx.assets->getFont(true));
    m_title.setString("Patient Medical History");
    m_title.setCharacterSize(28);
    m_title.setFillColor(sf::Color::White);
    m_title.setPosition(60.f, 82.f);
    m_inputPatientID.init(60.f, 130.f, 280.f, 46.f, "Patient ID", *ctx.assets);
    m_btnSearch.init(354.f, 130.f, 130.f, 46.f, "Search", *ctx.assets);
    string hdrs[] = { "Date","Medicines","Notes" };
    m_table.init(60.f, 192.f, 1800.f, 550.f, hdrs, 3, *ctx.assets);
    m_btnBack.init(60.f, 762.f, 140.f, 42.f, "Back", *ctx.assets, false);
    m_msgBox.init(*ctx.assets);
}
void DoctorPatientHistory::handleEvent(const sf::Event& e, GUIContext& ctx) {
    sf::Vector2f mp = ctx.mousePos;
    m_inputPatientID.handleEvent(e, *ctx.window, mp);
    m_btnSearch.handleEvent(e, *ctx.window, mp);
    m_table.handleEvent(e, mp); m_btnBack.handleEvent(e, *ctx.window, mp);
    m_msgBox.handleEvent(e, *ctx.window, mp);
    if (m_btnSearch.isClicked()) {
        m_btnSearch.resetClick(); m_table.clearRows();
        int pid = strToInt(m_inputPatientID.getText());
        Patient* p = FileHandler::getInstance()->loadPatients().findByID(pid);
        if (!p) { m_msgBox.show("Access Denied", "Patient not found."); return; }
        bool isOwn = false;
        Storage<Appointment>& appts = FileHandler::getInstance()->loadAppointments();
        for (int i = 0; i < appts.size(); i++) {
            Appointment* a = appts.getAt(i);
            if (a && a->getPatientID() == pid && a->getDoctorID() == ctx.loggedInID && Validator::stringEqual(a->getStatus(), "completed")) { isOwn = true; break; }
        }
        if (!isOwn) { m_msgBox.show("Access Denied", "You can only view your own patients."); return; }
        Storage<Prescription>& prescs = FileHandler::getInstance()->loadPrescriptions();
        static Prescription* arr[200]; int cnt = 0;
        for (int i = 0; i < prescs.size() && cnt < 200; i++) {
            Prescription* pr = prescs.getAt(i);
            if (pr && pr->getPatientID() == pid && pr->getDoctorID() == ctx.loggedInID) arr[cnt++] = pr;
        }
        for (int i = 0; i < cnt - 1; i++)
            for (int j = 0; j < cnt - i - 1; j++)
                if (Validator::dateToInt(arr[j]->getDate()) < Validator::dateToInt(arr[j + 1]->getDate())) {
                    Prescription* t = arr[j]; arr[j] = arr[j + 1]; arr[j + 1] = t;
                }
        for (int i = 0; i < cnt; i++) {
            string row[] = { arr[i]->getDate(),arr[i]->getMedicines(),arr[i]->getNotes() };
            m_table.addRow(row);
        }
        if (cnt == 0) m_msgBox.show("Info", "No prescriptions found for this patient.");
    }
    if (m_btnBack.isClicked()) { m_btnBack.resetClick(); ctx.nextScreen = ScreenID::DOCTOR_DASHBOARD; }
}
void DoctorPatientHistory::update(float, GUIContext&) {}
void DoctorPatientHistory::draw(sf::RenderWindow& win, GUIContext& ctx) {
    win.clear(Theme::BG);
    drawTopBar(win, ctx, ctx.loggedInName);
    win.draw(m_title);
    m_inputPatientID.draw(win);
    m_btnSearch.draw(win);
    m_table.draw(win);
    m_btnBack.draw(win);
    m_msgBox.draw(win);
}

// Admin Dashboard, centered tiles
void AdminDashboard::onEnter(GUIContext& ctx) {
    const string labels[TILE_COUNT] = { "Add Doctor","Add Patient","Remove Doctor","All Patients","All Doctors","Appointments","Unpaid Bills","Discharge","Security Log","Daily Report" };
    const string icons[TILE_COUNT] = { Assets::ICON_DOCTOR,Assets::ICON_PATIENT,Assets::ICON_DOCTOR,Assets::ICON_PATIENT,Assets::ICON_DOCTOR,Assets::ICON_APPT,Assets::ICON_BILLING,Assets::ICON_DISCHARGE,Assets::ICON_SECURITY,Assets::ICON_REPORTS };
    float tileSize = 180.f, gap = 26.f, totalW = 5 * (tileSize + gap) - gap;
    float sx = (1920.f - totalW) / 2.f, sy = 300.f;
    for (int i = 0; i < TILE_COUNT; i++) {
        int c = i % 5, r = i / 5;
        m_tiles[i].init(sx + c * (tileSize + gap), sy + r * (tileSize + gap + 20.f), tileSize, labels[i], icons[i], *ctx.assets);
    }
    m_btnLogout.init(1780.f, 15.f, 120.f, 42.f, "Logout", *ctx.assets, false);
    m_welcome.setFont(ctx.assets->getFont(true));
    m_welcome.setString("Admin Panel  MediCore");
    m_welcome.setCharacterSize(22);
    m_welcome.setFillColor(sf::Color::White);
    m_welcome.setPosition(320.f, 20.f);
}
void AdminDashboard::handleEvent(const sf::Event& e, GUIContext& ctx) {
    sf::Vector2f mp = ctx.mousePos;
    for (int i = 0; i < TILE_COUNT; i++) m_tiles[i].handleEvent(e, *ctx.window, mp);
    m_btnLogout.handleEvent(e, *ctx.window, mp);
    ScreenID targets[TILE_COUNT] = { ScreenID::ADMIN_ADD_DOCTOR,ScreenID::ADMIN_ADD_PATIENT,ScreenID::ADMIN_REMOVE_DOCTOR,ScreenID::ADMIN_ALL_PATIENTS,ScreenID::ADMIN_ALL_DOCTORS,ScreenID::ADMIN_ALL_APPTS,ScreenID::ADMIN_UNPAID_BILLS,ScreenID::ADMIN_DISCHARGE,ScreenID::ADMIN_SECURITY_LOG,ScreenID::ADMIN_DAILY_REPORT };
    for (int i = 0; i < TILE_COUNT; i++)
        if (m_tiles[i].isClicked()) { m_tiles[i].resetClick(); ctx.nextScreen = targets[i]; }
    if (m_btnLogout.isClicked()) { m_btnLogout.resetClick(); ctx.nextScreen = ScreenID::LOGIN; }
}
void AdminDashboard::update(float, GUIContext&) {}
void AdminDashboard::draw(sf::RenderWindow& win, GUIContext& ctx) {
    win.clear(Theme::BG);
    drawTopBar(win, ctx, "Admin");
    win.draw(m_welcome);
    for (int i = 0; i < TILE_COUNT; i++) m_tiles[i].draw(win);
    m_btnLogout.draw(win);
}

// Admin Add Doctor, navigate back after msgBox OK
void AdminAddDoctor::onEnter(GUIContext& ctx) {
    m_addedOK = false;
    // card: 480x520, centered on 1920x(1080-70)
    float cardW = 480.f, cardH = 520.f;
    float cardX = (1920.f - cardW) / 2.f;
    float cardY = 70.f + (1010.f - cardH) / 2.f;
    float cx = cardX + 40.f, w = 400.f, h = 46.f, gap = 58.f, sy = cardY + 68.f;
    m_title.setFont(ctx.assets->getFont(true));
    m_title.setString("Add New Doctor");
    m_title.setCharacterSize(28);
    m_title.setFillColor(sf::Color::White);
    m_title.setPosition(cx, cardY + 18.f);
    m_inputName.init(cx, sy, w, h, "Full Name", *ctx.assets);
    m_inputSpec.init(cx, sy + gap, w, h, "Specialization", *ctx.assets);
    m_inputContact.init(cx, sy + gap * 2, w, h, "Contact (11 digits)", *ctx.assets);
    m_inputPass.init(cx, sy + gap * 3, w, h, "Password (min 6)", *ctx.assets, true);
    m_inputFee.init(cx, sy + gap * 4, w, h, "Consultation Fee", *ctx.assets);
    m_btnAdd.init(cx + 100.f, sy + gap * 5 + 6.f, w - 200.f, 48.f, "Add Doctor", *ctx.assets);
    m_btnBack.init(cx + 100.f, sy + gap * 5 + 64.f, w - 200.f, 44.f, "Back", *ctx.assets, false);
    m_msgBox.init(*ctx.assets);
}
void AdminAddDoctor::handleEvent(const sf::Event& e, GUIContext& ctx) {
    sf::Vector2f mp = ctx.mousePos;
    m_inputName.handleEvent(e, *ctx.window, mp);
    m_inputSpec.handleEvent(e, *ctx.window, mp);
    m_inputContact.handleEvent(e, *ctx.window, mp);
    m_inputPass.handleEvent(e, *ctx.window, mp);
    m_inputFee.handleEvent(e, *ctx.window, mp);
    m_btnAdd.handleEvent(e, *ctx.window, mp);
    m_btnBack.handleEvent(e, *ctx.window, mp);
    m_msgBox.handleEvent(e, *ctx.window, mp);

    // FIX: navigate back once user clicks OK on success
    if (m_addedOK && m_msgBox.confirmedOK()) {
        m_msgBox.reset();
        m_addedOK = false;
        ctx.nextScreen = ScreenID::ADMIN_DASHBOARD;
        return;
    }

    if (m_btnAdd.isClicked()) {
        m_btnAdd.resetClick();
        try {
            if (!Validator::isValidName(m_inputName.getText()))    throw InvalidInputException("Invalid name.");
            if (!Validator::isValidContact(m_inputContact.getText())) throw InvalidInputException("Contact must be 11 digits.");
            if (!Validator::isValidPassword(m_inputPass.getText())) throw InvalidInputException("Password min 6 chars.");
            if (!Validator::isPositiveFloat(m_inputFee.getText().c_str())) throw InvalidInputException("Fee must be positive.");
            int newID = FileHandler::getInstance()->getMaxDoctorID() + 1;
            float fee = strToFloat(m_inputFee.getText());
            Doctor d(newID, m_inputName.getText(), m_inputSpec.getText(), m_inputContact.getText(), m_inputPass.getText(), fee);
            FileHandler::getInstance()->appendDoctor(d);
            m_addedOK = true;
            m_msgBox.show("Success", "Doctor added. ID: " + intToStr(newID) + "\nPress OK to return.");
        }
        catch (InvalidInputException& ex) { m_msgBox.show("Validation Error", ex.what()); }
    }
    if (m_btnBack.isClicked()) { m_btnBack.resetClick(); ctx.nextScreen = ScreenID::ADMIN_DASHBOARD; }
}
void AdminAddDoctor::update(float, GUIContext&) {}
void AdminAddDoctor::draw(sf::RenderWindow& win, GUIContext& ctx) {
    win.clear(Theme::BG);
    drawTopBar(win, ctx, "Admin");
    float cardY_d = 70.f + (1010.f - 520.f) / 2.f;
    drawCard(win, 720.f, cardY_d, 480.f, 520.f, sf::Color(18, 26, 46), sf::Color(30, 100, 200), 2.f);
    win.draw(m_title);
    m_inputName.draw(win);
    m_inputSpec.draw(win);
    m_inputContact.draw(win);
    m_inputPass.draw(win);
    m_inputFee.draw(win);
    m_btnAdd.draw(win);
    m_btnBack.draw(win);
    m_msgBox.draw(win);
}

// Admin Add Patient,navigate back after OK
void AdminAddPatient::onEnter(GUIContext& ctx) {
    m_addedOK = false;
    // card: 480x600, centered on 1920x(1080-70)
    float cardW = 480.f, cardH = 600.f;
    float cardX = (1920.f - cardW) / 2.f;
    float cardY = 70.f + (1010.f - cardH) / 2.f;
    float cx = cardX + 40.f, w = 400.f, h = 46.f, gap = 56.f, sy = cardY + 66.f;
    m_title.setFont(ctx.assets->getFont(true));
    m_title.setString("Add New Patient");
    m_title.setCharacterSize(28);
    m_title.setFillColor(sf::Color::White);
    m_title.setPosition(cx, cardY + 18.f);
    m_inputName.init(cx, sy, w, h, "Full Name", *ctx.assets);
    m_inputAge.init(cx, sy + gap, w, h, "Age", *ctx.assets);
    m_inputGender.init(cx, sy + gap * 2, w, h, "Gender (M or F)", *ctx.assets);
    m_inputContact.init(cx, sy + gap * 3, w, h, "Contact (11 digits)", *ctx.assets);
    m_inputPass.init(cx, sy + gap * 4, w, h, "Password (min 6)", *ctx.assets, true);
    m_inputBalance.init(cx, sy + gap * 5, w, h, "Initial Balance(PKR)", *ctx.assets);
    m_btnAdd.init(cx + 100.f, sy + gap * 6 + 6.f, w - 200.f, 48.f, "Add Patient", *ctx.assets);
    m_btnBack.init(cx + 100.f, sy + gap * 6 + 64.f, w - 200.f, 44.f, "Back", *ctx.assets, false);
    m_msgBox.init(*ctx.assets);
}
void AdminAddPatient::handleEvent(const sf::Event& e, GUIContext& ctx) {
    sf::Vector2f mp = ctx.mousePos;
    m_inputName.handleEvent(e, *ctx.window, mp);
    m_inputAge.handleEvent(e, *ctx.window, mp);
    m_inputGender.handleEvent(e, *ctx.window, mp);
    m_inputContact.handleEvent(e, *ctx.window, mp);
    m_inputPass.handleEvent(e, *ctx.window, mp);
    m_inputBalance.handleEvent(e, *ctx.window, mp);
    m_btnAdd.handleEvent(e, *ctx.window, mp);
    m_btnBack.handleEvent(e, *ctx.window, mp);
    m_msgBox.handleEvent(e, *ctx.window, mp);

    // FIX: navigate back once user clicks OK on success
    if (m_addedOK && m_msgBox.confirmedOK()) {
        m_msgBox.reset();
        m_addedOK = false;
        ctx.nextScreen = ScreenID::ADMIN_DASHBOARD;
        return;
    }

    if (m_btnAdd.isClicked()) {
        m_btnAdd.resetClick();
        try {
            if (!Validator::isValidName(m_inputName.getText()))    throw InvalidInputException("Invalid name.");
            if (!Validator::isValidContact(m_inputContact.getText())) throw InvalidInputException("Contact must be 11 digits.");
            if (!Validator::isValidPassword(m_inputPass.getText())) throw InvalidInputException("Password min 6 chars.");
            if (!Validator::isPositiveFloat(m_inputBalance.getText().c_str())) throw InvalidInputException("Balance must be positive.");
            string gStr = m_inputGender.getText();
            if (gStr.empty() || (gStr[0] != 'M' && gStr[0] != 'F' && gStr[0] != 'm' && gStr[0] != 'f'))
                throw InvalidInputException("Gender must be M or F.");
            int newID = FileHandler::getInstance()->getMaxPatientID() + 1;
            int age = stoi(m_inputAge.getText());
            char gender = (gStr[0] == 'm' || gStr[0] == 'M') ? 'M' : 'F';
            float bal = strToFloat(m_inputBalance.getText());
            Patient pat(newID, m_inputName.getText(), age, gender, m_inputContact.getText(), m_inputPass.getText(), bal);
            FileHandler::getInstance()->appendPatient(pat);
            m_addedOK = true;
            m_msgBox.show("Success", "Patient added. ID: " + intToStr(newID) + "\nPress OK to return.");
            m_inputName.clear();
            m_inputAge.clear();
            m_inputGender.clear();
            m_inputContact.clear();
            m_inputPass.clear();
            m_inputBalance.clear();
        }
        catch (InvalidInputException& ex) { m_msgBox.show("Validation Error", ex.what()); }
        catch (...) { m_msgBox.show("Error", "Failed. Check all fields."); }
    }
    if (m_btnBack.isClicked()) { m_btnBack.resetClick(); ctx.nextScreen = ScreenID::ADMIN_DASHBOARD; }
}
void AdminAddPatient::update(float, GUIContext&) {}
void AdminAddPatient::draw(sf::RenderWindow& win, GUIContext& ctx) {
    win.clear(Theme::BG);
    drawTopBar(win, ctx, "Admin");
    float cardY_p = 70.f + (1010.f - 600.f) / 2.f;
    drawCard(win, 720.f, cardY_p, 480.f, 600.f, sf::Color(18, 26, 46), sf::Color(30, 100, 200), 2.f);
    win.draw(m_title);
    m_inputName.draw(win);
    m_inputAge.draw(win);
    m_inputGender.draw(win);
    m_inputContact.draw(win);
    m_inputPass.draw(win);
    m_inputBalance.draw(win);
    m_btnAdd.draw(win);
    m_btnBack.draw(win);
    m_msgBox.draw(win);
}

// Admin Remove Doctor
void AdminRemoveDoctor::onEnter(GUIContext& ctx) {
    m_title.setFont(ctx.assets->getFont(true));
    m_title.setString("Remove Doctor");
    m_title.setCharacterSize(28);
    m_title.setFillColor(sf::Color::White);
    m_title.setPosition(60.f, 82.f);
    string hdrs[] = { "ID","Name","Specialization","Fee" };
    m_table.init(60.f, 126.f, 1800.f, 480.f, hdrs, 4, *ctx.assets);
    Storage<Doctor>& docs = FileHandler::getInstance()->loadDoctors();
    for (int i = 0; i < docs.size(); i++) {
        Doctor* d = docs.getAt(i);
        if (!d) continue;
        string row[] = { intToStr(d->getID()),d->getName(),d->getSpec(),floatToStr(d->getFee()) };
        m_table.addRow(row);
    }
    m_inputID.init(60.f, 626.f, 240.f, 46.f, "Doctor ID", *ctx.assets);
    m_btnRemove.init(314.f, 626.f, 140.f, 46.f, "Remove", *ctx.assets);
    m_btnBack.init(60.f, 688.f, 140.f, 42.f, "Back", *ctx.assets, false);
    m_msgBox.init(*ctx.assets);
}
void AdminRemoveDoctor::handleEvent(const sf::Event& e, GUIContext& ctx) {
    sf::Vector2f mp = ctx.mousePos;
    m_table.handleEvent(e, mp);
    m_inputID.handleEvent(e, *ctx.window, mp);
    m_btnRemove.handleEvent(e, *ctx.window, mp);
    m_btnBack.handleEvent(e, *ctx.window, mp);
    m_msgBox.handleEvent(e, *ctx.window, mp);
    if (m_btnRemove.isClicked()) { m_btnRemove.resetClick(); m_msgBox.show("Confirm", "Remove this doctor?", true); }
    if (m_msgBox.confirmedOK()) {
        m_msgBox.reset();
        int docID = strToInt(m_inputID.getText());
        Storage<Appointment>& appts = FileHandler::getInstance()->loadAppointments();
        bool hasPending = false;
        for (int i = 0; i < appts.size(); i++) {
            Appointment* a = appts.getAt(i);
            if (a && a->getDoctorID() == docID && Validator::stringEqual(a->getStatus(), "pending")) { hasPending = true; break; }
        }
        if (hasPending) { m_msgBox.show("Error", "Cannot remove doctor with pending appointments."); return; }
        FileHandler::getInstance()->deleteDoctor(docID);
        m_msgBox.show("Done", "Doctor removed.");
        m_table.clearRows();
        Storage<Doctor>& docs = FileHandler::getInstance()->loadDoctors();
        for (int i = 0; i < docs.size(); i++) {
            Doctor* d = docs.getAt(i);
            if (!d) continue;
            string row[] = { intToStr(d->getID()),d->getName(),d->getSpec(),floatToStr(d->getFee()) };
            m_table.addRow(row);
        }
    }
    if (m_btnBack.isClicked()) { m_btnBack.resetClick(); ctx.nextScreen = ScreenID::ADMIN_DASHBOARD; }
}
void AdminRemoveDoctor::update(float, GUIContext&) {}
void AdminRemoveDoctor::draw(sf::RenderWindow& win, GUIContext& ctx) {
    win.clear(Theme::BG);
    drawTopBar(win, ctx, "Admin");
    win.draw(m_title);
    m_table.draw(win);
    m_inputID.draw(win);
    m_btnRemove.draw(win);
    m_btnBack.draw(win);
    m_msgBox.draw(win);
}

// Admin View Patients
void AdminViewPatients::onEnter(GUIContext& ctx) {
    m_title.setFont(ctx.assets->getFont(true));
    m_title.setString("All Patients");
    m_title.setCharacterSize(28);
    m_title.setFillColor(sf::Color::White);
    m_title.setPosition(60.f, 82.f);
    string hdrs[] = { "ID","Name","Age","Gender","Contact","Balance","Unpaid Bills" };
    m_table.init(60.f, 126.f, 1800.f, 600.f, hdrs, 7, *ctx.assets);
    Storage<Patient>& pats = FileHandler::getInstance()->loadPatients();
    Storage<Bill>& bills = FileHandler::getInstance()->loadBills();
    for (int i = 0; i < pats.size(); i++) {
        Patient* p = pats.getAt(i); if (!p) continue;
        int unpaid = 0;
        for (int j = 0; j < bills.size(); j++) {
            Bill* b = bills.getAt(j);
            if (b && b->getPatientID() == p->getID() && Validator::stringEqual(b->getStatus(), "unpaid")) unpaid++;
        }
        string gender(1, p->getGender());
        string row[] = { intToStr(p->getID()),p->getName(),intToStr(p->getAge()),gender,p->getContact(),floatToStr((float)p->getBalance()),intToStr(unpaid) };
        m_table.addRow(row);
    }
    m_btnBack.init(60.f, 750.f, 140.f, 42.f, "Back", *ctx.assets, false);
}
void AdminViewPatients::handleEvent(const sf::Event& e, GUIContext& ctx) {
    sf::Vector2f mp = ctx.mousePos;
    m_table.handleEvent(e, mp); m_btnBack.handleEvent(e, *ctx.window, mp);
    if (m_btnBack.isClicked()) { m_btnBack.resetClick(); ctx.nextScreen = ScreenID::ADMIN_DASHBOARD; }
}
void AdminViewPatients::update(float, GUIContext&) {}
void AdminViewPatients::draw(sf::RenderWindow& win, GUIContext& ctx) {
    win.clear(Theme::BG);
    drawTopBar(win, ctx, "Admin");
    win.draw(m_title);
    m_table.draw(win);
    m_btnBack.draw(win);
}

// Admin View Doctors
void AdminViewDoctors::onEnter(GUIContext& ctx) {
    m_title.setFont(ctx.assets->getFont(true));
    m_title.setString("All Doctors");
    m_title.setCharacterSize(28);
    m_title.setFillColor(sf::Color::White);
    m_title.setPosition(60.f, 82.f);
    string hdrs[] = { "ID","Name","Specialization","Contact","Fee" };
    m_table.init(60.f, 126.f, 1800.f, 600.f, hdrs, 5, *ctx.assets);
    Storage<Doctor>& docs = FileHandler::getInstance()->loadDoctors();
    for (int i = 0; i < docs.size(); i++) {
        Doctor* d = docs.getAt(i); if (!d) continue;
        string row[] = { intToStr(d->getID()),d->getName(),d->getSpec(),d->getContact(),floatToStr(d->getFee()) };
        m_table.addRow(row);
    }
    m_btnBack.init(60.f, 750.f, 140.f, 42.f, "Back", *ctx.assets, false);
}
void AdminViewDoctors::handleEvent(const sf::Event& e, GUIContext& ctx) {
    sf::Vector2f mp = ctx.mousePos;
    m_table.handleEvent(e, mp); m_btnBack.handleEvent(e, *ctx.window, mp);
    if (m_btnBack.isClicked()) { m_btnBack.resetClick(); ctx.nextScreen = ScreenID::ADMIN_DASHBOARD; }
}
void AdminViewDoctors::update(float, GUIContext&) {}
void AdminViewDoctors::draw(sf::RenderWindow& win, GUIContext& ctx) {
    win.clear(Theme::BG);
    drawTopBar(win, ctx, "Admin");
    win.draw(m_title);
    m_table.draw(win);
    m_btnBack.draw(win);
}

// Admin All Appointments
void AdminAllAppts::onEnter(GUIContext& ctx) {
    m_title.setFont(ctx.assets->getFont(true));
    m_title.setString("All Appointments");
    m_title.setCharacterSize(28);
    m_title.setFillColor(sf::Color::White);
    m_title.setPosition(60.f, 82.f);
    string hdrs[] = { "ID","Patient","Doctor","Date","Slot","Status" };
    m_table.init(60.f, 126.f, 1800.f, 600.f, hdrs, 6, *ctx.assets);
    Storage<Appointment>& appts = FileHandler::getInstance()->loadAppointments();
    static Appointment* arr[500]; int cnt = 0;
    for (int i = 0; i < appts.size() && cnt < 500; i++) arr[cnt++] = appts.getAt(i);
    for (int i = 0; i < cnt - 1; i++)
        for (int j = 0; j < cnt - i - 1; j++)
            if (Validator::dateToInt(arr[j]->getDate()) < Validator::dateToInt(arr[j + 1]->getDate())) {
                Appointment* t = arr[j]; arr[j] = arr[j + 1]; arr[j + 1] = t;
            }
    for (int i = 0; i < cnt; i++) {
        Patient* p = FileHandler::getInstance()->loadPatients().findByID(arr[i]->getPatientID());
        Doctor* d = FileHandler::getInstance()->loadDoctors().findByID(arr[i]->getDoctorID());
        string row[] = { intToStr(arr[i]->getID()),p ? string(p->getName()) : "?",d ? string(d->getName()) : "?",arr[i]->getDate(),arr[i]->getTimeSlot(),arr[i]->getStatus() };
        m_table.addRow(row);
    }
    m_btnBack.init(60.f, 750.f, 140.f, 42.f, "Back", *ctx.assets, false);
}
void AdminAllAppts::handleEvent(const sf::Event& e, GUIContext& ctx) {
    sf::Vector2f mp = ctx.mousePos;
    m_table.handleEvent(e, mp); m_btnBack.handleEvent(e, *ctx.window, mp);
    if (m_btnBack.isClicked()) { m_btnBack.resetClick(); ctx.nextScreen = ScreenID::ADMIN_DASHBOARD; }
}
void AdminAllAppts::update(float, GUIContext&) {}
void AdminAllAppts::draw(sf::RenderWindow& win, GUIContext& ctx) {
    win.clear(Theme::BG);
    drawTopBar(win, ctx, "Admin");
    win.draw(m_title);
    m_table.draw(win);
    m_btnBack.draw(win);
}

// Admin Unpaid Bills
void AdminUnpaidBills::onEnter(GUIContext& ctx) {
    m_title.setFont(ctx.assets->getFont(true));
    m_title.setString("Unpaid Bills");
    m_title.setCharacterSize(28);
    m_title.setFillColor(sf::Color::White);
    m_title.setPosition(60.f, 82.f);
    string hdrs[] = { "Bill ID","Patient","Amount (PKR)","Date" };
    m_table.init(60.f, 126.f, 1800.f, 600.f, hdrs, 4, *ctx.assets);
    time_t now = time(nullptr);
    Storage<Bill>& bills = FileHandler::getInstance()->loadBills();
    for (int i = 0; i < bills.size(); i++) {
        Bill* b = bills.getAt(i);
        if (!b || !Validator::stringEqual(b->getStatus(), "unpaid")) continue;
        Patient* p = FileHandler::getInstance()->loadPatients().findByID(b->getPatientID());
        string dateStr = b->getDate();
        tm bt = {}; bt.tm_mday = Validator::extractDay(dateStr); bt.tm_mon = Validator::extractMonth(dateStr) - 1; bt.tm_year = Validator::extractYear(dateStr) - 1900;
        time_t billTime = mktime(&bt);
        double days = difftime(now, billTime) / 86400.0;
        string displayDate = dateStr + (days > 7.0 ? " [OVERDUE]" : "");
        string row[] = { intToStr(b->getID()),p ? string(p->getName()) : "Unknown",floatToStr(b->getAmount()),displayDate };
        m_table.addRow(row);
    }
    m_btnBack.init(60.f, 750.f, 140.f, 42.f, "Back", *ctx.assets, false);
}
void AdminUnpaidBills::handleEvent(const sf::Event& e, GUIContext& ctx) {
    sf::Vector2f mp = ctx.mousePos;
    m_table.handleEvent(e, mp); m_btnBack.handleEvent(e, *ctx.window, mp);
    if (m_btnBack.isClicked()) { m_btnBack.resetClick(); ctx.nextScreen = ScreenID::ADMIN_DASHBOARD; }
}
void AdminUnpaidBills::update(float, GUIContext&) {}
void AdminUnpaidBills::draw(sf::RenderWindow& win, GUIContext& ctx) {
    win.clear(Theme::BG);
    drawTopBar(win, ctx, "Admin");
    win.draw(m_title);
    m_table.draw(win);
    m_btnBack.draw(win);
}

// Admin Discharge
void AdminDischarge::onEnter(GUIContext& ctx) {
    m_title.setFont(ctx.assets->getFont(true));
    m_title.setString("Discharge Patient");
    m_title.setCharacterSize(28);
    m_title.setFillColor(sf::Color::White);
    m_title.setPosition(60.f, 82.f);
    string hdrs[] = { "ID","Name","Balance","Unpaid Bills","Pending Appts" };
    m_table.init(60.f, 126.f, 1800.f, 480.f, hdrs, 5, *ctx.assets);
    Storage<Patient>& pats = FileHandler::getInstance()->loadPatients();
    Storage<Bill>& bills = FileHandler::getInstance()->loadBills();
    Storage<Appointment>& appts = FileHandler::getInstance()->loadAppointments();
    for (int i = 0; i < pats.size(); i++) {
        Patient* p = pats.getAt(i); if (!p) continue;
        int unpaid = 0, pending = 0;
        for (int j = 0; j < bills.size(); j++) { Bill* b = bills.getAt(j); if (b && b->getPatientID() == p->getID() && Validator::stringEqual(b->getStatus(), "unpaid")) unpaid++; }
        for (int j = 0; j < appts.size(); j++) { Appointment* a = appts.getAt(j); if (a && a->getPatientID() == p->getID() && Validator::stringEqual(a->getStatus(), "pending")) pending++; }
        string row[] = { intToStr(p->getID()),p->getName(),floatToStr((float)p->getBalance()),intToStr(unpaid),intToStr(pending) };
        m_table.addRow(row);
    }
    m_inputID.init(60.f, 626.f, 240.f, 46.f, "Patient ID", *ctx.assets);
    m_btnDischarge.init(314.f, 626.f, 160.f, 46.f, "Discharge", *ctx.assets);
    m_btnBack.init(60.f, 688.f, 140.f, 42.f, "Back", *ctx.assets, false);
    m_msgBox.init(*ctx.assets);
}
void AdminDischarge::handleEvent(const sf::Event& e, GUIContext& ctx) {
    sf::Vector2f mp = ctx.mousePos;
    m_table.handleEvent(e, mp);
    m_inputID.handleEvent(e, *ctx.window, mp);
    m_btnDischarge.handleEvent(e, *ctx.window, mp);
    m_btnBack.handleEvent(e, *ctx.window, mp);
    m_msgBox.handleEvent(e, *ctx.window, mp);
    if (m_btnDischarge.isClicked()) { m_btnDischarge.resetClick(); m_msgBox.show("Confirm", "Discharge and archive this patient?", true); }
    if (m_msgBox.confirmedOK()) {
        m_msgBox.reset();
        int pid = strToInt(m_inputID.getText());
        Patient* p = FileHandler::getInstance()->loadPatients().findByID(pid);
        if (!p) { m_msgBox.show("Error", "Patient not found."); return; }
        if (FileHandler::getInstance()->hasUnpaidBills(pid)) { m_msgBox.show("Error", "Cannot discharge: unpaid bills."); return; }
        if (FileHandler::getInstance()->hasPendingAppts(pid)) { m_msgBox.show("Error", "Cannot discharge: pending appointments."); return; }
        FileHandler::getInstance()->dischargePatient(pid);
        m_msgBox.show("Done", "Patient discharged and archived.");
    }
    if (m_btnBack.isClicked()) { m_btnBack.resetClick(); ctx.nextScreen = ScreenID::ADMIN_DASHBOARD; }
}
void AdminDischarge::update(float, GUIContext&) {}
void AdminDischarge::draw(sf::RenderWindow& win, GUIContext& ctx) {
    win.clear(Theme::BG);
    drawTopBar(win, ctx, "Admin");
    win.draw(m_title);
    m_table.draw(win);
    m_inputID.draw(win);
    m_btnDischarge.draw(win);
    m_btnBack.draw(win);
    m_msgBox.draw(win);
}

// Admin SecurityLog
void AdminSecurityLog::onEnter(GUIContext& ctx) {
    m_title.setFont(ctx.assets->getFont(true));
    m_title.setString("Security Log");
    m_title.setCharacterSize(28);
    m_title.setFillColor(sf::Color::White);
    m_title.setPosition(60.f, 82.f);
    string hdrs[] = { "Timestamp","Role","Entered ID","Result" };
    m_table.init(60.f, 126.f, 1800.f, 600.f, hdrs, 4, *ctx.assets);
    FileHandler::getInstance()->loadSecurityLog(m_table);
    m_btnBack.init(60.f, 750.f, 140.f, 42.f, "Back", *ctx.assets, false);
}
void AdminSecurityLog::handleEvent(const sf::Event& e, GUIContext& ctx) {
    sf::Vector2f mp = ctx.mousePos;
    m_table.handleEvent(e, mp); m_btnBack.handleEvent(e, *ctx.window, mp);
    if (m_btnBack.isClicked()) { m_btnBack.resetClick(); ctx.nextScreen = ScreenID::ADMIN_DASHBOARD; }
}
void AdminSecurityLog::update(float, GUIContext&) {}
void AdminSecurityLog::draw(sf::RenderWindow& win, GUIContext& ctx) {
    win.clear(Theme::BG);
    drawTopBar(win, ctx, "Admin");
    win.draw(m_title);
    m_table.draw(win);
    m_btnBack.draw(win);
}

// Admin Daily Report
void AdminDailyReport::onEnter(GUIContext& ctx) {
    m_title.setFont(ctx.assets->getFont(true));
    m_title.setString("Daily Report");
    m_title.setCharacterSize(28);
    m_title.setFillColor(sf::Color::White);
    m_title.setPosition(60.f, 82.f);
    string h1[] = { "Status","Count" };
    m_tableSummary.init(60.f, 126.f, 400.f, 220.f, h1, 2, *ctx.assets);
    string h2[] = { "Patient","Total Owed (PKR)" };
    m_tableOutstanding.init(500.f, 126.f, 500.f, 220.f, h2, 2, *ctx.assets);
    string h3[] = { "Doctor","Completed","Pending","No-show" };
    m_tableDocSummary.init(60.f, 370.f, 1800.f, 280.f, h3, 4, *ctx.assets);
    m_revenueText.setFont(ctx.assets->getFont(true));
    m_revenueText.setCharacterSize(20);
    m_revenueText.setFillColor(Theme::SUCCESS);
    m_revenueText.setPosition(1060.f, 140.f);
    m_btnBack.init(60.f, 680.f, 140.f, 42.f, "Back", *ctx.assets, false);
    string today = getTodayStr();
    int pending = 0, completed = 0, noshow = 0, cancelled = 0; float revenue = 0.f;
    Storage<Appointment>& appts = FileHandler::getInstance()->loadAppointments();
    Storage<Bill>& bills = FileHandler::getInstance()->loadBills();
    Storage<Patient>& pats = FileHandler::getInstance()->loadPatients();
    Storage<Doctor>& docs = FileHandler::getInstance()->loadDoctors();
    for (int i = 0; i < appts.size(); i++) {
        Appointment* a = appts.getAt(i);
        if (!a || !Validator::stringEqual(a->getDate(), today)) continue;
        if (Validator::stringEqual(a->getStatus(), "pending"))   pending++;
        else if (Validator::stringEqual(a->getStatus(), "completed")) completed++;
        else if (Validator::stringEqual(a->getStatus(), "noshow"))    noshow++;
        else if (Validator::stringEqual(a->getStatus(), "cancelled")) cancelled++;
    }
    for (int i = 0; i < bills.size(); i++) {
        Bill* b = bills.getAt(i);
        if (b && Validator::stringEqual(b->getStatus(), "paid") && Validator::stringEqual(b->getDate(), today)) revenue += b->getAmount();
    }
    string r1[] = { "Pending",  intToStr(pending) };   m_tableSummary.addRow(r1);
    string r2[] = { "Completed",intToStr(completed) };  m_tableSummary.addRow(r2);
    string r3[] = { "No-show",  intToStr(noshow) };     m_tableSummary.addRow(r3);
    string r4[] = { "Cancelled",intToStr(cancelled) };  m_tableSummary.addRow(r4);
    m_revenueText.setString("Revenue Today:\nPKR " + floatToStr(revenue));
    for (int i = 0; i < pats.size(); i++) {
        Patient* p = pats.getAt(i); if (!p) continue;
        float owed = 0.f;
        for (int j = 0; j < bills.size(); j++) { Bill* b = bills.getAt(j); if (b && b->getPatientID() == p->getID() && Validator::stringEqual(b->getStatus(), "unpaid")) owed += b->getAmount(); }
        if (owed <= 0.f) continue;
        string row[] = { p->getName(),floatToStr(owed) }; m_tableOutstanding.addRow(row);
    }
    for (int i = 0; i < docs.size(); i++) {
        Doctor* d = docs.getAt(i); if (!d) continue;
        int dc = 0, dp = 0, dn = 0;
        for (int j = 0; j < appts.size(); j++) {
            Appointment* a = appts.getAt(j);
            if (!a || a->getDoctorID() != d->getID() || !Validator::stringEqual(a->getDate(), today)) continue;
            if (Validator::stringEqual(a->getStatus(), "completed")) dc++;
            else if (Validator::stringEqual(a->getStatus(), "pending"))  dp++;
            else if (Validator::stringEqual(a->getStatus(), "noshow"))   dn++;
        }
        if (dc + dp + dn == 0) continue;
        string row[] = { d->getName(),intToStr(dc),intToStr(dp),intToStr(dn) };
        m_tableDocSummary.addRow(row);
    }
}
void AdminDailyReport::handleEvent(const sf::Event& e, GUIContext& ctx) {
    sf::Vector2f mp = ctx.mousePos;
    m_tableSummary.handleEvent(e, mp);
    m_tableOutstanding.handleEvent(e, mp);
    m_tableDocSummary.handleEvent(e, mp);
    m_btnBack.handleEvent(e, *ctx.window, mp);
    if (m_btnBack.isClicked()) { m_btnBack.resetClick(); ctx.nextScreen = ScreenID::ADMIN_DASHBOARD; }
}
void AdminDailyReport::update(float, GUIContext&) {}
void AdminDailyReport::draw(sf::RenderWindow& win, GUIContext& ctx) {
    win.clear(Theme::BG);
    drawTopBar(win, ctx, "Admin");
    win.draw(m_title);
    m_tableSummary.draw(win);
    m_tableOutstanding.draw(win);
    m_tableDocSummary.draw(win);
    win.draw(m_revenueText);
    m_btnBack.draw(win);
}

// GUI App
GUIApp::GUIApp()
    :m_window(sf::VideoMode(1280, 720), "MediCore HMS", sf::Style::Default),
    m_current(nullptr), m_currentID(ScreenID::SPLASH) {
    m_window.setView(sf::View(sf::FloatRect(0.f, 0.f, 1920.f, 1080.f)));
    m_window.setVerticalSyncEnabled(true);
    m_ctx.window = &m_window; m_ctx.assets = &m_assets;
    m_ctx.nextScreen = ScreenID::SPLASH; m_ctx.loggedInID = -1;
    m_ctx.role = 0; m_ctx.loggedInName = ""; m_ctx.patientBalance = 0.f;
    m_ctx.selectedID = -1; m_ctx.mousePos = { 0.f,0.f };
    FileHandler::getInstance()->loadAll();
    switchScreen(ScreenID::SPLASH);
}
Screen* GUIApp::screenFor(ScreenID id) {
    switch (id) {
    case ScreenID::SPLASH: return &m_splash;
    case ScreenID::LOGIN: return &m_login;
    case ScreenID::PATIENT_DASHBOARD: return &m_patDash;
    case ScreenID::PATIENT_BOOK_APPT: return &m_patBook;
    case ScreenID::PATIENT_MY_APPTS: return &m_patAppts;
    case ScreenID::PATIENT_RECORDS: return &m_patRecords;
    case ScreenID::PATIENT_BILLS: return &m_patBills;
    case ScreenID::PATIENT_PAY_BILL: return &m_patBills;
    case ScreenID::PATIENT_TOPUP: return &m_patTopUp;
    case ScreenID::DOCTOR_DASHBOARD: return &m_docDash;
    case ScreenID::DOCTOR_TODAY_APPTS: return &m_docToday;
    case ScreenID::DOCTOR_MARK_COMPLETE: return &m_docToday;
    case ScreenID::DOCTOR_WRITE_PRESC: return &m_docPresc;
    case ScreenID::DOCTOR_PATIENT_HISTORY: return &m_docHistory;
    case ScreenID::ADMIN_DASHBOARD: return &m_adminDash;
    case ScreenID::ADMIN_ADD_DOCTOR: return &m_adminAddDoc;
    case ScreenID::ADMIN_ADD_PATIENT: return &m_adminAddPat;
    case ScreenID::ADMIN_REMOVE_DOCTOR: return &m_adminRemDoc;
    case ScreenID::ADMIN_ALL_PATIENTS: return &m_adminPats;
    case ScreenID::ADMIN_ALL_DOCTORS: return &m_adminDocs;
    case ScreenID::ADMIN_ALL_APPTS: return &m_adminAppts;
    case ScreenID::ADMIN_UNPAID_BILLS: return &m_adminBills;
    case ScreenID::ADMIN_DISCHARGE: return &m_adminDischarge;
    case ScreenID::ADMIN_SECURITY_LOG: return &m_adminLog;
    case ScreenID::ADMIN_DAILY_REPORT: return &m_adminReport;
    default: return &m_login;
    }
}
void GUIApp::switchScreen(ScreenID id) {
    m_currentID = id; m_current = screenFor(id);
    m_ctx.nextScreen = id; m_current->onEnter(m_ctx);
}
void GUIApp::run() {
    sf::Clock clock;
    sf::SoundBuffer buffer;
    buffer.loadFromFile("assets/audio/BackgroundMusic.wav");

    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.setLoop(true); //sound loop infinitely
    sound.play();
    while (m_window.isOpen()) {
        float dt = clock.restart().asSeconds();
        //updates mapped mouse position once per frame
        m_ctx.mousePos = getMappedMousePos(m_window);
        sf::Event e;
        while (m_window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) m_window.close();
            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape)
                m_window.close();
            if (e.type == sf::Event::Resized)
                m_window.setView(sf::View(sf::FloatRect(0.f, 0.f, 1920.f, 1080.f)));
            if (m_current) m_current->handleEvent(e, m_ctx);
        }
        if (m_ctx.nextScreen == ScreenID::EXIT) { m_window.close(); break; }
        if (m_ctx.nextScreen != m_currentID) switchScreen(m_ctx.nextScreen);
        if (m_current) {
            m_current->update(dt, m_ctx);
            m_window.clear(Theme::BG);
            m_current->draw(m_window, m_ctx);
            m_window.display();
        }
    }
    FileHandler::getInstance()->saveAll();
}